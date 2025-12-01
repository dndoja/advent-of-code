package main

import (
	"adventofcode23/day11"
	"adventofcode23/day12"
	"bufio"
	"fmt"
	"os"
	"strconv"
)

type LineReader = func(string)

func main() {
	args := os.Args[1:]
	if len(args) == 0 {
		fmt.Println("You must pass the dayNumber argument")
		return
	}

	dayNumber, err := strconv.Atoi(args[0])
	if err != nil {
		fmt.Println("You need to pass a valid day number")
		return
	}

	variation := ""
	if len(args) > 1 {
		variation = args[1]
	}

	fmt.Printf("Running day %d"+variation+"\n", dayNumber)

	file, err := os.Open(fmt.Sprintf("data/day%d"+variation+".txt", dayNumber))
	if err != nil {
		fmt.Println("Couldn't open input file")
		return
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)

	switch dayNumber {
	case 1:
		RunDay1(scanner)
	case 2:
		RunDay2(scanner)
	case 3:
		RunDay3(scanner)
	case 4:
		RunDay4(scanner)
	case 5:
		RunDay5(scanner)
	case 6:
		RunDay6(scanner)
	case 7:
		RunDay7(scanner)
	case 8:
		RunDay8(scanner)
	case 9:
		RunDay9(scanner)
	case 10:
		RunDay10(scanner)
	case 11:
		day11.Run(scanner)
	case 12:
		day12.Run(scanner)
	default:
		fmt.Println("This day is not yet implemented")
	}
}
