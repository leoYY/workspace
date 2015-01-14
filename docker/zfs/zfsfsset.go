// zfs filesystem set

package zfs

import (
	"fmt"	
	"sync"
	"path"
	"os/exec"
	
	"github.com/docker/docker/pkg/log"
)

var (
	DEFAULT_POOL_SIZE	int64 = 100 * 1024 * 1024 * 1024
	DEFAULT_QUTA_SIZE	uint64 = 20 * 1024 * 1024 * 1024
)

type SnapshotInfo struct {
	// pool/fs
	name 	string
	// @ver
	ver	string
}

type ZfsLayerInfo struct {
	// just id like imageId or containerId
	name		string	
	// just like pool_name/fsname
	zfsFsName 	string 
	// if not mount yet , mountPath == ""
	mountPath	string
	availSize	uint64
	referSize	uint64	
	// base layer name  
	baseName	string
	cloneCnt	int64
	snapInfo	*SnapshotInfo
	lock		sync.Mutex
}

type ZfsFsSet struct {
	root		string			
	layerInfos	map[string]*ZfsLayerInfo	
	fsSetLock	sync.Mutex
	mountPath	string
	poolName	string
	// options
	poolSize 	int64
	fsQutaSize	uint64
}

func NewZfsHandler(root string, options []string) (*ZfsFsSet, error) {
	log.Debugf("New ZfsHandler %s", root)
	zfsHandler := &ZfsFsSet {
		root:	root,
		layerInfos:	make(map[string]*ZfsLayerInfo),
		mountPath:	path.Join(root, "data"),
		poolName:	"docker_pool",
		poolSize:	DEFAULT_POOL_SIZE,
		fsQutaSize:	DEFAULT_QUTA_SIZE,
	}	
	
	log.Debugf("zfs create pool")
	err := zfsHandler.createPool() 
	if err != nil {
		log.Debugf("zfs create pool failed")
		return nil, err
	}
	err = zfsHandler.setupBaseImage()
	if err != nil {
		return nil, err
	}
	return zfsHandler, nil
}

func (zfsHandler *ZfsFsSet) setupBaseImage() error {
	layinfo := zfsHandler.lookupFsLayer("")
	if layinfo == nil {
		layinfo = &ZfsLayerInfo {
			name : "",
			zfsFsName: 	zfsHandler.poolName + "/base",
			mountPath: "",
			availSize: 0,	
			referSize: 0,
			baseName: "",
			snapInfo: nil,
		}
		err := CreateZfsFs(layinfo.zfsFsName)
		if err != nil {
			log.Debugf("create base Zfs fs failed")
			return nil
		}
		zfsHandler.layerInfos[layinfo.name] = layinfo	
	}
	return nil
}

func (zfsHandler *ZfsFsSet) createPool() error {
	filedir := zfsHandler.root
	imgFileDir := path.Join(filedir, "image")
	imgFileName := "zpool.img"
	imgFileSize := zfsHandler.poolSize	

	err, _ := ZpoolRun("list", zfsHandler.poolName)
	if err == nil {
		log.Debugf("reuse pool")
		return nil
	}

	log.Debugf("create img file")
	err = CreateImgFile(imgFileDir, imgFileName, imgFileSize)
	if err != nil {
		log.Debugf("create img file failed")
		return err
	}

	log.Debugf("Get loop Device")
	loopDev, err := GetAvailableLoopDevice()	
	if err != nil {
		log.Debugf("there is no available loop device")
		return err
	}

	imgFile := path.Join(imgFileDir, imgFileName)
	err = LoopDeviceSetup(loopDev, imgFile)			
	if err != nil {
		log.Debugf("loop device setup failed")
		return err
	}

	// @TODO need add error rollback if any step failed	
	log.Debugf("Create zfs pool")
	err = CreateZfsPool(zfsHandler.poolName, zfsHandler.mountPath, loopDev)		
	if err != nil {
		log.Debugf("create zfs pool failed")
		return err
	}
	return nil
}

func (zfsHandler *ZfsFsSet) lookupFsLayer(hash string) *ZfsLayerInfo {
	// @NOTE all layer meta data in mem, maybe do persistence
	layerinfoPtr, founded := zfsHandler.layerInfos[hash]
	if founded {
		return layerinfoPtr
	}
	return nil
}

func (zfsHandler *ZfsFsSet) loadLayerInfoData(datapath string) error {
	return nil
}

func (zfsHandler *ZfsFsSet) saveLayerInfoData(datapath string) error {
	return nil
}

func (zfsHandler *ZfsFsSet) mountFs(id string, mountPath string) error {
	log.Debugf("mount %s %s", id, mountPath)
	zfsHandler.fsSetLock.Lock()	
	defer zfsHandler.fsSetLock.Unlock()
	layinfo := zfsHandler.lookupFsLayer(id)
	if layinfo == nil {
		return fmt.Errorf("Unknown layer %s", id)
	}
	
	err := exec.Command("mount", "-t", "zfs", layinfo.zfsFsName, mountPath).Run() 
	if err != nil {
		return err
	}
	
	layinfo.mountPath = mountPath
	return nil
}

func (zfsHandler *ZfsFsSet) unmountFs(id string) error {
	log.Debugf("umount %s", id)
	zfsHandler.fsSetLock.Lock()	
	defer zfsHandler.fsSetLock.Unlock()
	layinfo := zfsHandler.lookupFsLayer(id)
	if layinfo == nil {
		return fmt.Errorf("Unknown layer %s", id)
	}
	
	if layinfo.mountPath == "" {
		log.Debugf("%s not mount yet", layinfo.zfsFsName)
		return nil
	}
	
	err := exec.Command("umount", layinfo.zfsFsName).Run()
	if err != nil  {
		log.Debugf("unmount %s failed", layinfo.zfsFsName)
		return err
	}
	layinfo.mountPath = ""
	return nil
}


func (zfsHandler *ZfsFsSet) destroyFs(id string) error {
	log.Debugf("destroy filesystem %s", id)
	// if not exists return nil 	
	zfsHandler.fsSetLock.Lock()	
	defer zfsHandler.fsSetLock.Unlock()
	
	layinfo := zfsHandler.lookupFsLayer(id)	
	if layinfo == nil {
		return nil
	}
	
	err := DestroyZfsClone(layinfo.zfsFsName)			
	if err != nil {
		return err
	}
	// @FIXME golang will GC layinfo ?
	zfsHandler.layerInfos[id] = nil
	return nil
}

func (zfsHandler *ZfsFsSet) createFs(home string, base string) error {
	log.Debugf("create %s base on %s", home, base)
	// @FIXME maybe lock scope is too big
	zfsHandler.fsSetLock.Lock()
	defer zfsHandler.fsSetLock.Unlock()

	// @FIXME how to rollback
	// @NOTE base == "" should build on SetupBaseImage
	layinfo := zfsHandler.lookupFsLayer(home)
	if layinfo != nil  {
		return nil
	}
	
	baseLayinfo := zfsHandler.lookupFsLayer(base)
	if baseLayinfo == nil {
		return fmt.Errorf("Unknown layer %s", base)
  	}
	
	// Step 1 snapshot base Filesystem, snapshot is readonly
	// @NOTE no need lock baseLay itself because of fsSetLock
	if baseLayinfo.snapInfo == nil {
		snap := &SnapshotInfo {
			name: baseLayinfo.zfsFsName,	
			// @FIXME maybe a random num better
			ver : "@base",
		}				
		err := CreateSnapshot(baseLayinfo.zfsFsName, snap.name + snap.ver)
		if err != nil {
			log.Debugf("create snapshot failed %s", snap.name+snap.ver)
			return err
		}
		baseLayinfo.snapInfo = snap
	}

	// Step 2 setup NewLayerInfo
	newLayinfo := &ZfsLayerInfo {
		name : home,
		zfsFsName: path.Join(zfsHandler.poolName, home),
		mountPath :	path.Join(zfsHandler.mountPath, home),
		// status info should update by func Status
		availSize : 0,
		referSize : 0,
		
		baseName : baseLayinfo.name,
		snapInfo : nil,
	}

	// Step 3 Clone empty Filesystem
	log.Debugf("begin to create clone %s", baseLayinfo.snapInfo.name)
	err := CreateClone(baseLayinfo.snapInfo.name + baseLayinfo.snapInfo.ver , newLayinfo.zfsFsName)	
	if err != nil {
		log.Debugf("create clone failed %s", newLayinfo.zfsFsName)
		return err;			
	}

	// Step 4 umount @FIXME maybe create clone shouldn't mount directly
	log.Debugf("begin to umount %s", newLayinfo.zfsFsName)
	err = exec.Command("umount", newLayinfo.zfsFsName).Run()
	if err != nil  {
		log.Debugf("unmount %s failed", newLayinfo.zfsFsName)
		return err
	}
	newLayinfo.mountPath = ""
	log.Debugf("add create clone %s %s",newLayinfo.name, baseLayinfo.snapInfo.name)
	zfsHandler.layerInfos[newLayinfo.name] = newLayinfo		
	return nil
}
