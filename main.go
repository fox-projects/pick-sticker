package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"path/filepath"

	g "github.com/AllenDang/giu"
	"github.com/AllenDang/giu/imgui"
)

var (
	i                           = int32(125)
	cfgFilterText               = ""
	cfgResetFilterTextOnStartup = true
	cfgShowEditTags             = false
	cfgCopySize                 = int32(25)
)

var cfg = struct {
	StickerDir               string
	ImageSize                *int32
	FilterText               *string
	ResetFilterTextOnStartup *bool
	ShowEditTags             *bool
	CopySize                 *int32
}{
	StickerDir:               "/home/edwin/Pics/telegram-stickers",
	ImageSize:                &i,
	FilterText:               &cfgFilterText,
	ResetFilterTextOnStartup: &cfgResetFilterTextOnStartup,
	ShowEditTags:             &cfgShowEditTags,
	CopySize:                 &cfgCopySize,
}

func doWithSticker(cfgToml CfgToml, dataJson DataJson, packName string, stickerFile string) *g.RowWidget {
	imageSize := float32(*cfg.ImageSize)
	stickerPath := filepath.Join(cfg.StickerDir, packName, stickerFile)

	widgets := []g.Widget{
		g.Button(fmt.Sprintf("Copy %s from %s", stickerFile, packName)).OnClick(func() {
			copyFile(stickerPath)
		}),
		g.ImageWithFile(stickerPath).Size(imageSize, imageSize),
	}

	if *cfg.ShowEditTags {
		var text = ""
		widgets = append(widgets, g.InputTextMultiline("", &text).Size(-1, imageSize))
	}

	return g.Row(widgets...)
}

func stickerRowBuilder(cfgToml CfgToml, dataJson DataJson) []*g.RowWidget {
	// TODO: append less efficient
	var rowWidgets []*g.RowWidget

	rootStickerDir, err := ioutil.ReadDir(cfg.StickerDir)
	handle(err)

	for _, stickerDir := range rootStickerDir {
		// subdir
		if stickerDir.IsDir() {
			files, err := ioutil.ReadDir(filepath.Join(cfg.StickerDir, stickerDir.Name()))
			handle(err)

			// each file in each subdirectory
			for _, info := range files {
				// filter non images/animations
				ext := filepath.Ext(info.Name())

				if !(ext == ".png" || ext == ".tgs") {
					continue
				}

				packName := stickerDir.Name()
				stickerFile := info.Name()

				rowWidgets = append(rowWidgets, doWithSticker(cfgToml, dataJson, packName, stickerFile))
			}

		}

	}

	return rowWidgets
}

// type CustomWidget struct {
// 	g.Widget
// }

// func (w *CustomWidget) Build() {
// }

func loop(cfgToml CfgToml, dataJson DataJson) {
	g.SingleWindow("Sticker Selector").Flags(g.WindowFlagsNoTitleBar | g.WindowFlagsNoResize | g.WindowFlagsNoMove | g.WindowFlagsNoCollapse | g.WindowFlagsAlwaysAutoResize).Layout(
		g.TabBar("Bar").Layout(
			g.TabItem("Stickers").Layout(
				g.Button("Quit").OnClick(func() {
					os.Exit(0)
				}),
				g.SliderInt("Size", cfg.ImageSize, 50, 500),
				g.SliderInt("Copy Size", cfg.CopySize, 50, 100),
				g.InputText("Filter", cfg.FilterText),
				g.Table("Sticker Table").FastMode(true).Rows(stickerRowBuilder(cfgToml, dataJson)...),
			),
			g.TabItem("Settings").Layout(
				g.Checkbox("Show Edit Tags", cfg.ShowEditTags),
				g.Label("Sticker Dir"),
				g.Label(cfg.StickerDir),
				g.Button("Change Dir").OnClick(func() {
					newDir := chooseDirDialog()
					cfg.StickerDir = newDir
				}),
			),
		),
	)
}

func main() {
	cfgToml := getCfg()
	dataJson := getData()

	imgui.CreateContext(nil)
	imgui.CurrentIO().Fonts().AddFontFromFileTTF("/home/edwin/Docs/z-fonts/fonts-1/ubuntu-font-family/UbuntuMono-B.ttf", 16)

	wnd := g.NewMasterWindow("Hello world", 600, 200, g.MasterWindowFlagsNotResizable|g.MasterWindowFlagsMaximized|g.MasterWindowFlagsFloating|g.MasterWindowFlagsFrameless|g.MasterWindowFlagsTransparent, nil)
	go updateMasterWindow(wnd)

	wnd.Run(func() {
		loop(cfgToml, dataJson)
	})
}
