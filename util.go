package main

import (
	"fmt"
	"os/exec"
	"strings"

	g "github.com/AllenDang/giu"
)

func handle(err error) {
	if err != nil {
		panic(err)
	}
}

func chooseDirDialog() string {
	cmd := exec.Command("sh", "-c", "cd folder-hack && go run .")
	output, err := cmd.CombinedOutput()
	handle(err)

	return strings.TrimSpace(string(output))
}

func copyFile(file string) {
	go func() {
		cmd := exec.Command("sh", "-c", fmt.Sprintf("convert %s -resize 100x100 - | xclip -selection clipboard -target image/png", file))
		// cmd := exec.Command("xclip", "-selection", "clipboard", "-target", "image/png", file)
		output, err := cmd.CombinedOutput()
		if err != nil {
			fmt.Println(err)
		}
		if cmd.ProcessState.ExitCode() != 0 {
			fmt.Println(string(output))
		}
	}()
}

func updateMasterWindow(wnd *g.MasterWindow) {
	// XgetScreenDimensions()

	// wnd.SetSize(400, 600)
	sizeX, sizeY := wnd.GetSize()
	posX, posY := wnd.GetPos()

	// fmt.Println(sizeX, sizeY, posX, posY)

	wnd.SetSize(sizeX, sizeY+400+400)
	wnd.SetPos(posX, posY-400)
}
