package main

import (
	"encoding/json"
	"io/ioutil"
	"os"
	"path/filepath"
	"runtime"

	"github.com/pelletier/go-toml"
)

type CfgToml struct {
	File string
}

type DataJson struct {
	File2 string
}

func getCfgDir() string {
	var cfgDir string

	switch runtime.GOOS {
	case "darwin":
		cfgDir = filepath.Join(os.Getenv("HOME"), "Library", "Preferences")
	case "windows":
		cfgDir = os.Getenv("APPDATA")
	default:
		if os.Getenv("XDG_CONFIG_HOME") != "" {
			cfgDir = os.Getenv("XDG_CONFIG_HOME")
		} else {
			cfgDir = filepath.Join(os.Getenv("HOME"), ".config")
		}
	}

	return cfgDir
}

func getDataDir() string {
	var dataDir string

	switch runtime.GOOS {
	case "darwin":
		dataDir = filepath.Join(os.Getenv("HOME"), "Library", "Application Support")
	case "windows":
		dataDir = filepath.Join(os.Getenv("APPDATA"), "Roamin")
	default:
		if os.Getenv("XDG_DATA_HOME") != "" {
			dataDir = os.Getenv("XDG_DATA_HOME")
		} else {
			dataDir = filepath.Join(os.Getenv("HOME"), ".local", "share")
		}
	}

	return dataDir
}

func getCfg() CfgToml {
	cfgFile := filepath.Join(getCfgDir(), "sticker-selector", "settings.toml")
	var cfgToml CfgToml

	cfgText, err := ioutil.ReadFile(cfgFile)
	if os.IsNotExist(err) {
		text, err := toml.Marshal(cfgToml)
		handle(err)

		err = os.MkdirAll(filepath.Dir(cfgFile), 0o755)
		handle(err)

		err = ioutil.WriteFile(cfgFile, text, 0o644)
		handle(err)
	} else if err == nil {
		handle(err)
	} else {
		err = toml.Unmarshal(cfgText, &cfgToml)
		handle(err)
	}

	return cfgToml
}

func getData() DataJson {
	cfgFile := filepath.Join(getDataDir(), "sticker-selector", "data.json")
	var dataJson DataJson

	cfgText, err := ioutil.ReadFile(cfgFile)
	if os.IsNotExist(err) {
		text, err := json.Marshal(dataJson)
		handle(err)

		err = os.MkdirAll(filepath.Dir(cfgFile), 0o755)
		handle(err)

		err = ioutil.WriteFile(cfgFile, text, 0o644)
		handle(err)
	} else if err == nil {
		handle(err)
	} else {
		err = toml.Unmarshal(cfgText, &dataJson)
		handle(err)
	}

	return dataJson
}
