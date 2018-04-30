package main

import (
	"log"
	"os"
	"path"
)

var filenames = []string{
	"mar14_2018_test11.txt",
	"mar16_rosa1_upper_water_canyon1.txt",
	"mar17_rosa_hunt_ironwood1.txt",
	"mar18_rosa_trek_torry_pines1.txt",
}

func main() {
	for _, filename := range filenames {

		infile, err := os.Open("../../data/clean/" + filename)
		if err != nil {
			log.Fatal(err)
		}
		defer infile.Close()

		name := path.Base(filename)
		outfile, err := os.Create(name)
		if err != nil {
			log.Fatal(err)
		}
		defer infile.Close()

	}
}
