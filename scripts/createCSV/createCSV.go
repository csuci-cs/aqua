package main

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"os"
	"path"
	"strings"
)

var filenames = []string{
	"mar14_2018_test11.txt",
	"mar16_rosa1_upper_water_canyon1.txt",
	"mar17_rosa_hunt_ironwood1.txt",
	"mar18_rosa_trek_torry_pines1.txt",
}

var headers = []string{
	"timestamp", "o2", "co", "humidity", "temperature", "pressure", "altitude", "NH3", "CO", "NO2", "C3H8", "C4H10", "CH4", "H2", "C2H5OH", "co2", "dust",
}

func main() {
	for _, filename := range filenames {
		func(filename string) {
			lineNumber := 0 // for debugging
			defer func() {
				if re := recover(); re != nil {
					fmt.Printf("%s line number %d caused a panic\n", filename, lineNumber)
					panic(re)
				}
			}()

			infile, err := os.Open("../../data/clean/" + filename)
			if err != nil {
				log.Fatal(err)
			}
			defer infile.Close()

			name := path.Base(filename)
			outfile, err := os.Create(name + ".csv")
			if err != nil {
				log.Fatal(err)
			}
			defer infile.Close()

			fmt.Fprintln(outfile, strings.Join(headers, ","))

			r := bufio.NewReader(infile)

			for {
				rawFrame := make([]string, 17)

				for i, _ := range rawFrame {
					rawFrame[i], err = r.ReadString('\n')
					rawFrame[i] = strings.TrimSpace(rawFrame[i])
					lineNumber++
					if err != nil {
						if err == io.EOF {
							break
						}
						log.Fatalf("error near line %d: %s", lineNumber, err)
					}
				}

				if len(rawFrame[0]) == 0 {
					break
				}
				fmt.Fprintf(outfile, "%s,", rawFrame[0][:len("2018-03-14 13:50:34.942433")])

				for i, _ := range rawFrame[:len(rawFrame)-1] {
					outFmt := "%s,"
					if i == 15 {
						outFmt = "%s"
					}
					// fmt.Printf("%d: %s\n", lineNumber-(16-i), str)
					fmt.Fprintf(outfile, outFmt, cleanLine(rawFrame[i]))
				}
				fmt.Fprintln(outfile)
			}
		}(filename)
	}
}

var presufixset = map[string][2]string{
	"o2:":         [2]string{"2018-03-14 13:50:34.942433 o2:", ""},
	"co:":         [2]string{"2018-03-14 13:50:34.976224 co:", ""},
	"humidity":    [2]string{"2018-03-14 13:50:35.012079 humidity:", ""},
	"temperature": [2]string{"2018-03-14 13:50:35.048744 temperature:", ""},
	"pressure":    [2]string{"2018-03-14 13:50:35.079280 pressure:", ""},
	"altitude":    [2]string{"2018-03-14 13:50:35.106823 altitude:", ""},
	"NH3":         [2]string{"2018-03-14 13:50:35.134070 The concentration of NH3 is", "  ppm"},
	"CO":          [2]string{"2018-03-14 13:50:35.160973 The concentration of CO is", "  ppm"},
	"NO2":         [2]string{"2018-03-14 13:50:35.188639 The concentration of NO2 is", "  ppm"},
	"C3H8":        [2]string{"2018-03-14 13:50:35.217186 The concentration of C3H8 is", "  ppm"},
	"C4H10":       [2]string{"2018-03-14 13:50:35.244926 The concentration of C4H10 is", "  ppm"},
	"CH4":         [2]string{"2018-03-14 13:50:35.286967 The concentration of CH4 is", "  ppm"},
	"H2":          [2]string{"2018-03-14 13:50:35.323101 The concentration of H2 is", "  ppm"},
	"C2H5OH":      [2]string{"2018-03-14 13:50:35.368225 The concentration of C2H5OH is", "  ppm"},
	"co2:":        [2]string{"2018-03-14 13:50:35.444777 co2:", ""},
	"dust":        [2]string{"2018-03-14 13:50:35.991894 dust concentration =", "pcs/0.01cf"},
}

func cleanLine(str string) string {
	if strings.Contains(str, "ERROR") || str == "" {
		return ""
	}

	for key, filters := range presufixset {
		if !strings.Contains(str, key) {
			continue
		}
		trimFront := len(filters[0])
		trimEnd := len(str) - (len(filters[1]))
		return strings.TrimSpace(strings.Trim(str[trimFront:trimEnd], ": \n"))
	}
	return ""
}

// 2018-03-14 13:50:34.942433 o2: 15.28
// 2018-03-14 13:50:34.976224 co: 211
// 2018-03-14 13:50:35.012079 humidity: 17.00
// 2018-03-14 13:50:35.048744 temperature: 21.00
// 2018-03-14 13:50:35.079280 pressure: 1013.07
// 2018-03-14 13:50:35.106823 altitude: 0.2
// 2018-03-14 13:50:35.134070 The concentration of NH3 is 2.66 ppm
// 2018-03-14 13:50:35.160973 The concentration of CO is 24.17 ppm
// 2018-03-14 13:50:35.188639 The concentration of NO2 is 0.27 ppm
// 2018-03-14 13:50:35.217186 The concentration of C3H8 is 4453.71 ppm
// 2018-03-14 13:50:35.244926 The concentration of C4H10 is 2280.33 ppm
// 2018-03-14 13:50:35.286967 The concentration of CH4 is 349319.09 ppm
// 2018-03-14 13:50:35.323101 The concentration of H2 is 9.89 ppm
// 2018-03-14 13:50:35.368225 The concentration of C2H5OH is 15.34 ppm
// 2018-03-14 13:50:35.444777 co2: 565
// 2018-03-14 13:50:35.991894 dust concentration = 6119.95 pcs/0.01cf
