//+linux build

package zfs 

import (
	"fmt"
	"os/exec" 
	"bytes"
	"os"
	"path"
	"strings"
)


func ZpoolRun(args ...string) (error ,string){
	cmd := exec.Command("zpool", args...)
	var out bytes.Buffer
	var err_str bytes.Buffer
	cmd.Stdout = &out
	cmd.Stderr = &err_str
	err := cmd.Run()
	if err != nil {
		fmt.Println("zpool run failed [", err_str.String(), "]")
		return err, out.String()
	} else {
		return nil, out.String()
	}
}

func DestroyZfsClone(name string) error {
	err, _ := ZfsRun("destroy", name)
	return err
}

func CreateZfsFs(name string) error {
	err, _ := ZfsRun("create", name)
	return err
}

func CreateZfsPool(name string, mountPath string, device string) error {
	err, _ := ZpoolRun("list", name)
	if err == nil {
		fmt.Println("pool already exists")
		// pool already exists
		return nil
	}
	
	fmt.Println("zpool create doing")
	err, _ = ZpoolRun("create", "-m", mountPath, name, device)
	if err != nil {
		return err
	}
	return nil
}

func CreateSnapshot(fsname string, snap string) error {
	fmt.Println("create snap")
	defer fmt.Println("create snap finish")
	err, _ := ZfsRun("snapshot", snap)
	if err != nil {
		fmt.Println("create snapshot failed")
		return err
	} else {
		return nil
	}
}

func CreateClone(snapName string, home string) error {
	fmt.Println("create clone %s %s", snapName, home)
	defer fmt.Println("craete clone finish")
	err, _ := ZfsRun("clone", snapName, home)
	if err != nil {
		fmt.Println("create clone failed")
		return err
	} else {
		return nil
	}
}


func ZfsRun(args ...string) (error, string) {
	cmd := exec.Command("zfs", args...)
	var out bytes.Buffer
	var err_str bytes.Buffer
	cmd.Stdout = &out
	cmd.Stderr = &err_str
	err := cmd.Run()
	if err != nil {
		fmt.Println("zfs run failed [", err_str.String(), "]")
		return err, out.String()
	} else {
		return nil, out.String()
	}
}

func GetAvailableLoopDevice() (string, error){
	return "/dev/loop6", nil
}

func LoopDeviceInfo(loop string) string {
	cmd := exec.Command("losetup", loop)
	var out bytes.Buffer	
	cmd.Stdout = &out
	err := cmd.Run()
	if err != nil {
		// losetup loop has no info 
		return ""
	} else {
		return out.String()	
	}
	
}

func LoopDeviceSetup(loop string, imgFile string) error{
	loopInfo := LoopDeviceInfo(loop)
	if loopInfo != "" {
		if strings.Contains(loopInfo, imgFile) {
			return nil
		}
		return fmt.Errorf("Loop Device %s busy", loop)
	}
	cmd := exec.Command("losetup", loop, imgFile)	
	var out bytes.Buffer
	var err_str bytes.Buffer
	cmd.Stdout = &out
	cmd.Stderr = &err_str
	err := cmd.Run()
	if err != nil {
		fmt.Println("losetup failed [", err_str.String(), "]")
		return err
	} else {
		fmt.Println("losetup suc [", out.String(), "]")
		return nil
	}
}


func CreateImgFile(filePath string, fileName string, fileSize int64) error {
	if err := os.MkdirAll(filePath, 0700); err != nil && !os.IsExist(err) {
		return err
	}	
	file := path.Join(filePath, fileName)
	
	if _, err := os.Stat(file); err != nil {
		if !os.IsNotExist(err) {
			return err
		}
		//log.Debugf("Creating loopback file %s", filename)
		f, err := os.OpenFile(file, os.O_RDWR|os.O_CREATE, 0600)
		if err != nil {
			return err
		}
		defer f.Close()
		
		if err = f.Truncate(fileSize); err != nil {
			return err
		}
	}
	return nil
}

