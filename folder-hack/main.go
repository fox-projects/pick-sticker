package main

import (
	"fmt"

	"github.com/sqweek/dialog"
)

func main() {
	dir, err := dialog.Directory().Title("Select Directory").Browse()
	if err != nil {
		if err.Error() == "Cancelled" {
			fmt.Println("")
			return
		}

		panic(err)
	}

	fmt.Println(dir)
}
