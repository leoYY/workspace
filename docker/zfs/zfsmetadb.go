// +build linux

package zfs 

import (
	"database/sql"
	"os"
	"fmt"
	"sync"

	_ "code.google.com/p/gosqlite/sqlite3" // registers sqlite
	"github.com/docker/docker/pkg/log"
)

// db shema
const (
	createZfsFsSetTable = `
	CREATE TABLE IF NOT EXISTS fsSet (
		"root" text NOT NULL PRIMARY KEY,
		"mountPath" text NULL,
		"poolName" text NULL,
		"poolSize" int 0,
		"fsQuteSize" int 0		
	);`
	createLayerinfoTable = `
	CREATE TABLE IF NOT EXISTS layinfo (
		"name" text NOT NULL PRIMARY KEY,
		"zfsFsName" text NULL,
		"mountPath" text NULL,
		"availSize" int 0,
		"referSize" int 0,
		"baseName" text NULL,
		"cloneCnt" int 0,
		"snapInfoId" text NULL);`
	createSnapshotInfoTable = `
	CREATE TABLE IF NOT EXISTS snapinfo (
		"name" text NOT NULL PRIMARY KEY,
		"ver" text NULL);`
)

type MetaDbDriver struct {
	conn *sql.DB	
	mux	sync.RWMutex	
}

func NewSqliteConn(root string) (*MetaDbDriver, error) {
	initDatabase := false
	stat, err := os.Stat(root)
	if err != nil {
		if os.IsNotExist(err) {
			initDatabase = true
		} else {
			return nil, err
		}
	}
	
	if stat != nil && stat.Size() == 0 {
		initDatabase = true
	}

	conn, err := sql.Open("sqlite3", root)
	if err != nil {
		log.Debugf("open db failed")
		return nil, err
	}
	return NewMetaDbDriver(conn, initDatabase)
}

func NewMetaDbDriver (conn *sql.DB, init bool) (*MetaDbDriver, error) {
	if conn == nil {
		return nil, fmt.Errorf("Db connection is nil")
	}

	db := &MetaDbDriver {conn: conn}
	if init {
		// create table
		if _, err := conn.Exec(createZfsFsSetTable); err != nil {
			log.Debugf("create ZfsFsSet table failed")
			return nil, err
		}
		if _, err := conn.Exec(createLayerinfoTable); err != nil {
			log.Debugf("create LayerInfo Table failed")
			return nil, err
		}
		if _, err := conn.Exec(createSnapshotInfoTable); err != nil {
			log.Debugf("create SnapshotInfo Table failed")
			return nil, err
		}
	}	
	return db, nil
}

func (driver *MetaDbDriver) saveZfsFsSet(zfsHandler *ZfsFsSet) error {
	if _, err := driver.conn.Exec("INSERT INTO fsSet (root, mountPath, poolName, poolSize, fsQuteSize) VALUES (?,?,?,?,?)", zfsHandler.root, zfsHandler.mountPath, zfsHandler.poolName, zfsHandler.poolSize, zfsHandler.fsQuteSize); err != nil {
		log.Debugf("save ZfsFsSet failed")
		return err
	}
	return nil
}

func (driver *MetaDbDriver) loadZfsFsSet(root string) (*ZfsFsSet, error) {
	driver.mux.RLock()
	defer driver.mux.RUnlock()

	if rows, err := driver.conn.Query("SELECT mountPath, poolName, poolSize, fsQuteSize FROM fsSet WHERE root = ?;", root)
	if err != nil {
		if err == sql.ErrNoRows {
			// not err, just not save 
			return nil, nil
		}
		return nil, err
	}
	defer rows.Close()
	
	for rows.Next() {
		var mountPath string	
		var poolName string	
		var poolSize int64
		var fsQuteSize int64
		if err := rows.Scan(&mountPath, &poolName, &poolSize, &fsQuteSize); err != nil {
			log.Debugf("rows scan failed")
			return nil, err
		}
		zfsHandler := &ZfsFsSet {
			root: root,
			mountPath: mountPath,
			poolName: poolName,
			poolSize: poolSize,
			fsQuteSize: fsQuteSize,
		}	
		// it should not multi rows, if so only use first
		return zfsHandler, nil
	}	
	return nil, fmt.Errorf("table is empty")
}

func (driver *MetaDbDriver) updateLayerInfo(layInfo *ZfsLayerInfo) error {
	// save or update LayerInfo
	driver.mux.Lock()	
	defer driver.mux.Unlock()

	rollback := func() {
		driver.conn.Exec("ROLLBACK")
	}

	if _, err := driver.conn.Exec("BEGIN EXCLUSIVE"); err != nil {
		log.Debugf("begin exclusive failed")
		return err
	}

	if err := driver.conn.QueryRow("SELECT name FROM layinfo WHERE name = ?;", layinfo.name); err != nil {
		if err == sql.ErrNoRows {
			if _, err := driver.conn.Exec("INSERT INTO layinfo (name, zfsFsName, mountPath, availSize, referSize, baseName, cloneCnt, snapInfoId) VALUES(?,?,?,?,?,?,?,?);", layInfo.name, layInfo.zfsFsName, layInfo.mountPath, layInfo.availSize, layInfo.referSize, layInfo.baseName, layInfo.cloneCnt, layInfo.snapInfo.name); err != nil {
				log.Debugf("insert layinfo failed")
				rollback()			
				return err
			}
		} else {
			rollback()
			return err
		}
	} else {
		err = driver.conn.Exec("UPDATE layinfo SET zfsFsName = ?, mountPath = ?, availSize = ?, referSize = ?, baseName = ?, cloneCnt = ?, snapInfoId = ? WHERE name = ?;", layInfo.zfsFsName, layInfo.mountPath, layInfo.availSize, layInfo.referSize, layInfo.baseName, layInfo.cloneCnt, layInfo.snapInfo.name, layInfo.name)
		if err != nil {
			rollback()
			log.Debugf("update layinfo failed")
			return err;
		}
	}
	if err = driver.conn.Exec("COMMIT"); err != nil {
		return err;
	}
	return nil	
}

func (driver *MetaDbDriver) getLayerInfo(name string) (*ZfsLayerInfo, error) {
	driver.mux.RLock()
	defer driver.mux.RUnlock()
	
	if rows, err := driver.conn.Query("SELECT * FROM layinfo WHERE name = ?;", name); err != nil {
		if err == sql.ErrNoRows {
			return nil, nil
		} 
		log.Debugf("select layinfo failed")
		return nil, err
	} 	
	for rows.Next() {
		var zfsFsName string		
		var mountPath string
		var availSize uint64
		var referSize uint64
		var baseName string
		var cloneCnt int64
		var snapInfoId string
		if err := rows.Scan(&zfsFsName, &mountPath, &availSize, &referSize, &baseName, &cloneCnt, &snapInfoId) {
			log.Debugf("rows scan failed")
			return nil, err
		}
		zfsLayinfo := &ZfsLayerInfo{
			name: name,
			zfsFsName: zfsFsName,
			mountPath: mountPath,
			availSize: availSize,
			referSize: referSize,
			baseName: baseName,
			cloneCnt: cloneCnt,
			snapInfo: &SnapshotInfo {
				name: snapInfoId,
			},
		}
		return zfsLayinfo, nil
	}
	return nil, nil
} 

func (driver *MetaDbDriver) updateSnapshotInfo(snapInfo *SnapshotInfo) error {

}

func (driver *MetaDbDriver) getSnapshotInfo(name string) (*SnapshotInfo, error) {

}

func (driver *MetaDbDriver) close() error {
	return driver.conn.Close()
}
