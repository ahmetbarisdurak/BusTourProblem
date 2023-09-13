﻿// BusTour.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <StaticVectorLibrary.h>
#include <LinkedListLibrary.h>

#define TIME_COUNT 60

class Time {
public:
    int hour;
    int minute;

    Time() : hour(0), minute(0) {}

    Time(int h, int m) : hour(h), minute(m) {}

    Time(std::string input) {
        hour = (input[0] - '0') * 10 + (input[1] - '0');
        minute = (input[3] - '0') * 10 + (input[4] - '0') ;
    }
    int operator-(const Time& other) const {
        int thisMinutes = hour * 60 + minute;
        int otherMinutes = other.hour * 60 + other.minute;
        int diffMinutes = thisMinutes - otherMinutes;

        // Ensure the result is always positive
        if (diffMinutes < 0) {
            diffMinutes = -diffMinutes;
        }

        return diffMinutes;
    }

    Time operator+(int minutes) const {
        int total_minutes = hour * 60 + minute;
        total_minutes += minutes;

        int new_hour = total_minutes / 60;
        int new_minute = total_minutes % 60;

        if (new_hour >= 24) {
            new_hour %= 24; // Ensure the hour does not exceed 24
        }

        return Time(new_hour, new_minute);
    }

    // Overload the < operator to compare two Time objects
    bool operator<(const Time& other) const {
        if (hour < other.hour) {
            return true;
        } else if (hour == other.hour) {
            return minute < other.minute;
        } else {
            return false;
        }
    }

    // Overload the == operator to compare two Time objects for equality
    bool operator==(const Time& other) const {
        return (hour == other.hour) && (minute == other.minute);
    }

    int GetHour() {
        return hour;
    }
    
    int GetMinutes() {
        return minute;
    }

};

std::ostream& operator<<(std::ostream& os, Time& time) {
    os << "(" << time.GetHour() << ":" << time.GetMinutes() << ") ";
    return os;
}

void readInput(StaticVector<Time, TIME_COUNT>& busTimes) {

    std::string input[] = { "10.05", "10.29", "10.29", "10.36", "10.40", "10.41", "10.48", "10.53", "10.53", "10.57", "-1" };
    int i = 0;
    while (input[i].compare("-1") != 0 ) {
        Time temp;
        temp.hour = (input[i][0] - '0') * 10 + (input[i][1] - '0');
        temp.minute = (input[i][3] - '0') * 10 + (input[i][4] - '0');

        busTimes.PushBack(temp);
        i++;
    }
}

Time CreateBusStartTime(Time startTime, Time endTime) {

    bool checkTime = false;
    int startHour, startMinute;
    //std::cout << "Creating bus start time" << std::endl;

    while (!checkTime) {
        
        startHour = rand() % 24;
        startMinute = rand() % 60;

        Time busStartTime(startHour, startMinute);

        if (!(busStartTime < startTime) && busStartTime < endTime)
            return busStartTime;

    }
    
    return startTime;
}

void CreateStableBusPeriod(StaticVector<Time, TIME_COUNT>& busTimes, int busNumber, Time startTime, Time endTime) {

    int i = 0;
    while(i < busNumber) { // kaldırılabilir
        int busPeriod = rand() % 30;

        Time busStartTime = CreateBusStartTime(startTime, endTime);
        
        std::cout << "Bus start time is " << busStartTime << std::endl;

        if (endTime < busStartTime + busPeriod)
            continue;

        while (busStartTime == endTime || busStartTime < endTime) {
            busTimes.PushBack(busStartTime);
            std::cout << "Added" << busStartTime << std::endl;
            busStartTime = busStartTime + busPeriod;
        }

       
        std::cout << "Bus period is " << busPeriod << std::endl;
    
        //std::cout << busTimes;
        //std::cout << "\n--------------------------------" << std::endl;
        i++;

    }
}

void CreateAlternatingBusPeriod(StaticVector<Time, TIME_COUNT>& busTimes, int busNumber, Time startTime, Time endTime) {

    StaticVector<Time, TIME_COUNT> tempTimes;
    int busPeriod1 = -1, busPeriod2 = -1;

    while (tempTimes.GetSize() < 5) {

        tempTimes = StaticVector<Time, TIME_COUNT>();

        busPeriod1 = rand() % 15;
        busPeriod2 = rand() % 15;

        if (busPeriod1 == 0 || busPeriod2 == 0 || busPeriod1 == busPeriod2)
            continue;

        Time busStartTime = CreateBusStartTime(startTime, endTime);

        std::cout << "Bus start time is " << busStartTime << " ";

        if (endTime < busStartTime + busPeriod1)
            continue;

        
        while (busStartTime == endTime || busStartTime < endTime) {
            tempTimes.PushBack(busStartTime);
            busStartTime = busStartTime + busPeriod1;

            if (endTime < busStartTime)
                break;

            tempTimes.PushBack(busStartTime);
            busStartTime = busStartTime + busPeriod2;
        }

    }

    std::cout << "Bus period is " << busPeriod1 << " and " << busPeriod2 << std::endl;
    for (int i = 0; i < tempTimes.GetSize(); ++i) {
        busTimes.PushBack(tempTimes[i]);
        std::cout << "Added" << tempTimes[i] << std::endl;

    }
}

void CreateBusPeriod(StaticVector<Time, TIME_COUNT>& busTimes, int busNumber, Time startTime, Time endTime) {
    int i = 0;
    while (i < busNumber) {

        float temp = (float)rand() / RAND_MAX;

        
        if (temp > 0.5)
            CreateStableBusPeriod(busTimes, 1, startTime, endTime);
        else
            CreateAlternatingBusPeriod(busTimes, 1, startTime, endTime);
        
        i++;
    }



}

bool CompareTime(const Time& time1, const Time& time2) {
    return !(time1 < time2);
}



void FindAlternatingPeriod(StaticVector<Time, TIME_COUNT> busTimes, StaticVector<Time, TIME_COUNT> maximumPattern, StaticVector<Time, TIME_COUNT>& foundPattern) {
    //std::cout << "\n-------------------------------------------------" << std::endl;
    //std::cout << "Entering the gate with period: " << std::endl;
    //std::cout << "Current maximum pattern is " << maximumPattern;

    int period1, period2;
    int periods[20][2] = { -1 };
    bool visited[20][20] = { false };
    int alternatingPeriodCount = 0;

    if (maximumPattern.GetSize() <= 2) // This can't create a alternating pattern
    {
        //std::cout << "Size is low so can't take it " << std::endl;
        return;
    }
    for (int s = 0; s < maximumPattern.GetSize() - 1; s++) {
        for (int start = 0; start < busTimes.GetSize(); ++start) {

            if (busTimes[start] < maximumPattern[s] || busTimes[start] == maximumPattern[s]) // we should traverse between the values of maximum pattern
                continue;

            else if (maximumPattern[s + 1] < busTimes[start] || busTimes[start] == maximumPattern[s + 1]) // we should traverse between the values of maximum pattern
                break;

            period1 = busTimes[start] - maximumPattern[s];
            period2 = maximumPattern[s + 1] - busTimes[start];

            //std::cout << "Period 1 is: " << period1 << " Period 2 is: " << period2 << std::endl;

            if (period1 == period2)
                continue;

            bool found = false;

            for (int index = 0; index < alternatingPeriodCount; ++index) {
                if (periods[index][0] == period1 && periods[index][1] == period2) {
                    //std::cout << "this one already exists " << period1 << "   " << period2 << std::endl;
                    visited[index][s] = true;
                    found = true;
                }
            }

            if (!found) {

                //std::cout << "this one doesn't exists adding to periods " << period1 << "   " << period2 << std::endl;
                periods[alternatingPeriodCount][0] = period1;
                periods[alternatingPeriodCount][1] = period2;
                visited[alternatingPeriodCount][s] = true;
                alternatingPeriodCount++;
            }
        }
    }

    //std::cout << "Finding the most occured pattern in the alternatif patterns" << std::endl;
    //std::cout << "Alternatif period count is " << alternatingPeriodCount << std::endl;
    int foundMaxIndex = 0;

    for (int s = 0; s < alternatingPeriodCount; ++s) {
        foundMaxIndex = 0;

        for (int ss = 0; ss < maximumPattern.GetSize() - 1; ++ss) {
            if (!visited[s][ss]) {
                //std::cout << periods[s][0] << " and " << periods[s][1] << " not true " << std::endl;
                foundMaxIndex = -1;
                break;
            }
        }

        if (foundMaxIndex == 0) {
            foundMaxIndex = s;
            break;
        }
    }

   
    if (foundMaxIndex == -1)
        return;

    //std::cout << "Maximum one is " << periods[foundMaxIndex][0] << "   " << periods[foundMaxIndex][1] << std::endl;

    period1 = periods[foundMaxIndex][0];
    period2 = periods[foundMaxIndex][1];
    foundPattern.PushBack(maximumPattern[0]);   
    int periodCount = 1;
    int i = 0;
    Time nextPeriodPrediction = maximumPattern[0];
    while(nextPeriodPrediction < maximumPattern[maximumPattern.GetSize() - 1]) {

        nextPeriodPrediction = nextPeriodPrediction + period1;
        foundPattern.PushBack(nextPeriodPrediction);
        
        nextPeriodPrediction = nextPeriodPrediction + period2;
        foundPattern.PushBack(nextPeriodPrediction);
        
    }

}

void ExtractAlternatingPeriods(StaticVector<Time, TIME_COUNT> busTimes, bool visited[TIME_COUNT]) {

    StaticVector<StaticVector<Time, TIME_COUNT>, TIME_COUNT> alternatingPatternsGroup;
    int numberOfAlternatingPatterns = 0;

    StaticVector<Time, TIME_COUNT> maxAlternatingPattern;
    int count = 0;

    bool tempVisited[TIME_COUNT] = { false };

    for (int i = 0; i < busTimes.GetSize(); ++i) {

        if (tempVisited[i]) // This time stamp is already visited
            continue;

        StaticVector<Time, TIME_COUNT> foundAlternatingPattern;

        for (int j = i + 1; j < busTimes.GetSize(); ++j) {

            if (tempVisited[j]) // A bus already passed in this time
                continue;

            int busPeriod = busTimes[j] - busTimes[i]; // Found a value for bus period to try.

            if (busPeriod == 0) // 0 is not accepted as period
                continue;

            int periodCount = 1;
            StaticVector<Time, TIME_COUNT> foundPattern;

            foundPattern.PushBack(busTimes[i]);
            foundPattern.PushBack(busTimes[j]);

            for (int k = j + 1; k < busTimes.GetSize(); ++k) {
                if (tempVisited[k])
                    continue;

                Time nextPeriodPrediction = busTimes[j] + busPeriod * periodCount;

                if (nextPeriodPrediction == busTimes[k]) { // If there is equality in pattern add it to pattern
                    foundPattern.PushBack(busTimes[k]);
                    //std::cout << "Found period is " << busPeriod << " and pushed " << busTimes[k] << std::endl;
                    periodCount++;
                }
            }

            
            foundAlternatingPattern = StaticVector<Time, TIME_COUNT>();

            FindAlternatingPeriod(busTimes, foundPattern, foundAlternatingPattern);
                
            if (foundAlternatingPattern.GetSize() > 0) {

                for (int j = 0; j < foundAlternatingPattern.GetSize(); ++j) {
                    for (int k = 0; k < busTimes.GetSize(); ++k) {
                        if (busTimes[k] == foundAlternatingPattern[j]) {
                            tempVisited[k] = true;
                            break;
                        }
                    }
                }

                // std::cout << "Found alternating pattern is " << foundAlternatingPattern;
                maxAlternatingPattern = foundAlternatingPattern;
                alternatingPatternsGroup[numberOfAlternatingPatterns] = foundAlternatingPattern;
                numberOfAlternatingPatterns++;
            }
            //else
                //std::cout << "No alternating pattern in this one " << std::endl;
        
        }
        // alternating patternda bulduğunu visited yap
        //std::cout << "Last found alternating pattern" << std::endl;
        //std::cout << maxAlternatingPattern;

        for (int j = 0; j < maxAlternatingPattern.GetSize(); ++j) {
            for (int k = 0; k < busTimes.GetSize(); ++k) {
                if (busTimes[k] == maxAlternatingPattern[j]) {
                    tempVisited[k] = true;
                    visited[k] = true;
                    break;
                }
            }
        }
    }


    std::cout << "All GROUPS " << std::endl;

    for (int j = 0; j < numberOfAlternatingPatterns; j++) {

        std::cout << "Group " << j << "period " << alternatingPatternsGroup[j][1] - alternatingPatternsGroup[j][0] << " and " << alternatingPatternsGroup[j][2] - alternatingPatternsGroup[j][1] << std::endl;
        std::cout << alternatingPatternsGroup[j] << std::endl;

    }


}

int main() {

    StaticVector<Time, TIME_COUNT> busTimes;
    bool visited[TIME_COUNT] = { false };


    srand(time(NULL));

    Time startTime(10, 0);
    Time endTime(11, 0);

    CreateBusPeriod(busTimes, 3, startTime, endTime);

    busTimes.Sort(CompareTime);

    std::cout << busTimes << std::endl;

    std::cout << "PERIODS ARE CREATED" << std::endl;

    std::cout << "TRYING TO FIND THE SAME PERIODS" << std::endl;

    
    //readInput(busTimes);

    ExtractAlternatingPeriods(busTimes, visited);

    std::cout << "Start doing the stable periods" << std::endl;

    for (int i = 0; i < busTimes.GetSize(); ++i) {

        if (visited[i]) // Node is already visited
            continue;


        StaticVector<Time, TIME_COUNT> maximumPattern;
        int maximumPeriodCount = 0;

        for (int j = i + 1; j < busTimes.GetSize(); ++j) {

            if (visited[j]) // Node is already visited 
                continue;

            int busPeriod = busTimes[j] - busTimes[i]; // Found a value for bus period to try.
            
            if(busPeriod == 0)
                continue;

            int periodCount = 1; // This was the possible first period 
            StaticVector<Time, TIME_COUNT> foundPattern;

            foundPattern.PushBack(busTimes[i]);
            foundPattern.PushBack(busTimes[j]);
            for (int k = j + 1; k < busTimes.GetSize(); ++k) {
                if (visited[k])
                    continue;

                Time nextPeriodPrediction = busTimes[j] + busPeriod * periodCount;

                if (nextPeriodPrediction < busTimes[k])  //If there is a contradiction stop operating
                    break;

                else if (nextPeriodPrediction == busTimes[k]) { // If there is equality in pattern add it to pattern
                    foundPattern.PushBack(busTimes[k]);
                    periodCount++;
                }
            }

            if (periodCount >= maximumPeriodCount) {
                maximumPeriodCount = periodCount;
                maximumPattern = foundPattern;
            }
        }

        int patternSize = 0;

        for (int k = 0; k < maximumPattern.GetSize(); ++k) {
            for (int l = 0; l < busTimes.GetSize(); ++l) {
                
                if (patternSize == maximumPattern.GetSize())
                    break;

                if (maximumPattern[k] == busTimes[l] && !visited[l]) {
                    visited[l] = true;
                    patternSize++;
                    break;
                }
            }
        }

        if(maximumPattern.GetSize() > 0)
            std::cout << "Period is:" << maximumPattern[1] - maximumPattern[0] << " Found pattern is " << maximumPattern << std::endl;
    }
    
    return 0;
}



