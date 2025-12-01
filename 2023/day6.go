package main

import (
	"bufio"
	"fmt"
)
import "errors"
import "math"
import "strconv"
import "strings"

type Race struct {
	time     int
	distance int
}

func RunDay6(scanner *bufio.Scanner) {
	lines := []string{}
	for scanner.Scan() {
		lines = append(lines, scanner.Text())
	}
	races := parseRaces(lines, false)
	mergedRace := parseRaces(lines, true)

	fmt.Println(getNumberOfWaysToWinRaces(races))
	fmt.Println(getNumberOfWaysToWinRaces(mergedRace))
}

func getNumberOfWaysToWinRaces(races []Race) int {
	waysToWinRace := 1

	for _, race := range races {
		minSpeed, maxSpeed, err := getSpeedRangeToWinRace(race)

		if err == nil {
			waysToWinRace *= maxSpeed - minSpeed + 1
		}
	}

	return waysToWinRace
}

// Speed equation:
// speed * (time - speed) >= recordDistance
// speed * (-speed + time) >= recordDistance
// -speed^2 + time * speed -recordDistance >= 0
func getSpeedRangeToWinRace(race Race) (int, int, error) {
	const a float64 = -1
	b := float64(race.time)
	c := -float64(race.distance)

	// The discriminant
	d := float64(b*b - 4*a*c)

	if d < 0 {
		return 0, 0, errors.New("No solution exists for this race")
	}

	x1 := (-b + math.Sqrt(d)) / 2 * a
	x2 := (-b - math.Sqrt(d)) / 2 * a

	return int(math.Ceil(x1)), int(math.Floor(x2)), nil
}

func parseRaces(lines []string, merge bool) []Race {
	timesRaw := strings.Fields(lines[0])[1:]
	distancesRaw := strings.Fields(lines[1])[1:]
	races := []Race{}

	mergedTime := ""
	mergedDistance := ""

	for i := 0; i < len(timesRaw); i++ {
		if merge {
			mergedTime += timesRaw[i]
			mergedDistance += distancesRaw[i]
		} else {
			time, timeErr := strconv.Atoi(timesRaw[i])
			dist, distErr := strconv.Atoi(distancesRaw[i])
			if timeErr == nil && distErr == nil {
				races = append(races, Race{time, dist})
			}
		}
	}

	if merge {
		time, timeErr := strconv.Atoi(mergedTime)
		dist, distErr := strconv.Atoi(mergedDistance)

		if timeErr == nil && distErr == nil {
			races = append(races, Race{time, dist})
		}
	}

	return races
}
