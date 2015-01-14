// +build linux

package zfs

import (
	"fmt"
	"path"	
	"runtime/debug"

	"github.com/docker/docker/daemon/graphdriver"
	"github.com/docker/docker/pkg/log"
)

func init() {
	graphdriver.Register("zfs", Init)	
}

type Driver struct {
	zfsFsSet *ZfsFsSet
	home string
}

func Init(home string, options []string) (graphdriver.Driver, error) {
	log.Debugf("init zfs driver [home :%s]", home)

	fsSet, err := NewZfsHandler(home, options)
	if err != nil {
		log.Debugf("zfs device New failed")
		return nil, err
	}
	d := &Driver{
		zfsFsSet : fsSet,
		home : home,
	}
	return graphdriver.NaiveDiffDriver(d), nil
}

func (d *Driver) String() string {
	return "zfs"
}

func (d *Driver) Cleanup() error {
	fmt.Println("Clean up")
	return nil
}

func (d *Driver) Status() [][2]string {
	status := [][2]string{}
	return status
}

func (d *Driver) Create(id, parent string) error {
	log.Debugf("zfs Create begin %s", id)
	if d.zfsFsSet == nil {
		return fmt.Errorf("zfs handler not inited yet")
	}

	err := d.zfsFsSet.createFs(id, parent)
	if err != nil {
		log.Debugf("zfsDriver Create failed")
		return err
	}
	return nil
}

func (d *Driver) Remove(id string) error {
	log.Debugf("driver remove %s", id)
	if d.zfsFsSet == nil {
		return fmt.Errorf("zfs handler not inited yet")
	}
	
	err := d.zfsFsSet.destroyFs(id)
	if err != nil {
		log.Debugf("zfsDriver Remove failed")
		return err
	}
	return nil
}

func (d *Driver) Get(id, mountLabel string) (string, error) {
	log.Debugf("driver get %s %s", id, mountLabel)
	if d.zfsFsSet == nil {
		return "", fmt.Errorf("zfs handler not inited yet")
	}
	
	if mountLabel == "" {
		mountLabel = path.Join(d.zfsFsSet.mountPath, id)
	}
	err := d.zfsFsSet.mountFs(id, mountLabel)
	if err != nil {
		return "", err
	}
	return mountLabel, nil
}

func (d *Driver) Put(id string) {
	debug.PrintStack()
	log.Debugf("driver put %s", id)
	if d.zfsFsSet == nil {
		return 
	}
	
	err := d.zfsFsSet.unmountFs(id)
	if err != nil {
		log.Debugf("umount %s failed", id)
	}
}

func (d *Driver) Exists(id string) bool {
	if d.zfsFsSet == nil {
		return false
	}
	
	layinfo := d.zfsFsSet.lookupFsLayer(id)	
	if layinfo == nil  {
		return false
	}
	return true
}

