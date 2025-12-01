package main

import (
	"bufio"
	"fmt"
)
import "strconv"
import "strings"

type IntOption struct {
	val   int
	isSet bool
}

func RunDay1(scanner *bufio.Scanner) {
	sumPt1 := 0
	sumPt2 := 0

	for scanner.Scan() {
		line := scanner.Text()
		sumPt1 += getCalibrationValue(line)
		sumPt2 += getCalibrationValue(normalizeInput(line))
	}

	fmt.Println(sumPt1)
	fmt.Println(sumPt2)
}

func getCalibrationValue(rawInput string) int {
	rightIndex := len(rawInput) - 1
	middle := rightIndex/2 + rightIndex%2

	var ll, lr, mid, rl, rr IntOption
	midNr, midErr := strconv.Atoi(string(rawInput[middle]))
	if midErr == nil {
		mid = IntOption{val: midNr, isSet: true}
	}

	for i := 0; i < middle; i++ {
		left := string(rawInput[i])
		right := string(rawInput[rightIndex-i])

		leftNr, leftErr := strconv.Atoi(left)
		if leftErr == nil {
			if !ll.isSet {
				ll = IntOption{val: leftNr, isSet: true}
			}
			lr = IntOption{val: leftNr, isSet: true}
		}

		rightNr, rightErr := strconv.Atoi(right)
		if rightErr == nil {
			if !rr.isSet {
				rr = IntOption{val: rightNr, isSet: true}
			}
			rl = IntOption{val: rightNr, isSet: true}
		}
	}

	var lhs int
	if ll.isSet {
		lhs = ll.val
	} else if mid.isSet {
		lhs = mid.val
	} else if rl.isSet {
		lhs = rl.val
	}

	var rhs int
	if rr.isSet {
		rhs = rr.val
	} else if mid.isSet {
		rhs = mid.val
	} else if lr.isSet {
		rhs = lr.val
	}

	return lhs*10 + rhs
}

func normalizeInput(rawInput string) string {
	if len(rawInput) <= 3 {
		switch rawInput {
		case "one":
			return "1"
		case "two":
			return "2"
		case "six":
			return "6"
		default:
			return rawInput
		}
	}

	var builder strings.Builder

	skippedAhead := false

	for i := 1; i < len(rawInput)-1; i++ {
		combined := rawInput[i-1 : i+2]

		var potentialMatch string

		switch combined {
		case "one":
			potentialMatch = "one"
		case "two":
			potentialMatch = "two"
		case "thr":
			potentialMatch = "three"
		case "fou":
			potentialMatch = "four"
		case "fiv":
			potentialMatch = "five"
		case "six":
			potentialMatch = "six"
		case "sev":
			potentialMatch = "seven"
		case "eig":
			potentialMatch = "eight"
		case "nin":
			potentialMatch = "nine"
		default:
			potentialMatch = ""
		}

		if skippedAhead {
			skippedAhead = false
		} else if i > 1 {
			builder.WriteString(string(rawInput[i-2]))
		}

		if potentialMatch != "" {
			lookaheadIndex := i + len(potentialMatch) - 2
			lookahead := rawInput[i-1 : lookaheadIndex+1]

			if potentialMatch == lookahead {
				switch lookahead {
				case "one":
					builder.WriteByte('1')
				case "two":
					builder.WriteByte('2')
				case "three":
					builder.WriteByte('3')
				case "four":
					builder.WriteByte('4')
				case "five":
					builder.WriteByte('5')
				case "six":
					builder.WriteByte('6')
				case "seven":
					builder.WriteByte('7')
				case "eight":
					builder.WriteByte('8')
				case "nine":
					builder.WriteByte('9')
				}

				if len(rawInput)-1-lookaheadIndex < 3 {
					builder.WriteString(rawInput[lookaheadIndex+1:])
					break
				}

				i = lookaheadIndex
				skippedAhead = true
			}
		} else if i == len(rawInput)-2 {
			builder.WriteString(combined)
		}
	}

	normalized := builder.String()

	return normalized
}
