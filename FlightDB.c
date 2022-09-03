
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "List.h"
#include "FlightDb.h"
#include "AVLTree.h"

#define LARGEST_AIRPORT_STR "~~~~~~~~"

struct flightDb {
	Tree byFlightNumber;
	Tree byDepartureAirportDay;
	Tree byTime;
};

static int compareByTime (Record r1, Record r2);
static int compareByDepartureAirportDay(Record r1, Record r2);
static int compareByFlightNumber(Record r1, Record r2);

//compare by day, hour, minute,flight number
static int compareByTime (Record r1, Record r2) {
	int compDay = RecordGetDepartureDay(r1) - RecordGetDepartureDay(r2);
	if (compDay != 0){
		return compDay;
	}
	else {
		int compHour = RecordGetDepartureHour(r1) - RecordGetDepartureHour(r2);
		if (compHour != 0) {
			return compHour;
		} 
		else {
			int compMin = RecordGetDepartureMinute(r1) - RecordGetDepartureMinute(r2);
			if (compMin != 0){
				return compMin;
			}
			return strcmp(RecordGetFlightNumber(r1),RecordGetFlightNumber(r2));
		}
	}
}

//comparison functions
//compare by airport, day, hour, minute, flight number
static int compareByDepartureAirportDay(Record r1, Record r2) {
	int airportComp =strcmp(RecordGetDepartureAirport(r1),RecordGetDepartureAirport(r2));
	if (airportComp != 0) {
		return airportComp;
	}
	else{
		return compareByTime (r1, r2);
	}
}

//compare by flight number, day, hour, min
static int compareByFlightNumber(Record r1, Record r2) {
	
	int compFlightNum = strcmp(RecordGetFlightNumber(r1),RecordGetFlightNumber(r2));
	if (compFlightNum != 0) {
		return compFlightNum;
	}
	else{
		int compDay = RecordGetDepartureDay(r1) - RecordGetDepartureDay(r2);
		if (compDay != 0){
			return compDay;
		}
		else {
			int compHour = RecordGetDepartureHour(r1) - RecordGetDepartureHour(r2);
			if (compHour != 0) {
				return compHour;
			} 
			else {
				int compMin = RecordGetDepartureMinute(r1) - RecordGetDepartureMinute(r2);
				
				return compMin;
				//if flightn number, day, hour and min are all the same, 0 is returned, and AVL tree will not insert
				
			}
		}
	}
}


/**
 * Creates a new flight DB. 
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
FlightDb DbNew(void) {
	FlightDb db = malloc(sizeof(*db));
    if (db == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
	db->byDepartureAirportDay = TreeNew(compareByDepartureAirportDay);
    db->byFlightNumber = TreeNew(compareByFlightNumber); 
	db->byTime = TreeNew(compareByTime);
    return db;
}

/**
 * Frees all memory allocated to the given flight DB
 */
void     DbFree(FlightDb db) {
	TreeFree(db->byFlightNumber, false);
	TreeFree(db->byTime, false);
    TreeFree(db->byDepartureAirportDay, true);
    free(db);
}

/**
 * Inserts  a  flight  record  into the given DB if there is not already
 * record with the same flight number, departure airport, day, hour  and
 * minute.
 * If  inserted successfully, this function takes ownership of the given 
 * record (so the caller should not modify or free it). 
 * Returns true if the record was successfully inserted,  and  false  if
 * the  DB  already  contained  a  record  with  the same flight number,
 * departure airport, day, hour and minute.
 * The time complexity of this function must be O(log n).
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
bool     DbInsertRecord(FlightDb db, Record r) {
	if (TreeInsert(db->byDepartureAirportDay, r)) {
        TreeInsert(db->byFlightNumber, r);
		TreeInsert(db->byTime, r);
        return true;
    } else {
        return false;
    }
}

/**
 * Searches  for  all  records with the given flight number, and returns
 * them all in a list in increasing order of  (day, hour, min).  Returns
 * an empty list if there are no such records. 
 * The  records  in the returned list should not be freed, but it is the
 * caller's responsibility to free the list itself.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
List     DbFindByFlightNumber(FlightDb db, char *flightNumber) {
	// Two dummies with the same flight number, and smallest and biggest time
	Record dummy1 = RecordNew(flightNumber, "", "", 0, 0, 0, 0);
	Record dummy2 = RecordNew(flightNumber, "", "", 6, 23, 59, 0);

    List l = TreeSearchBetween(db->byFlightNumber, dummy1, dummy2);
    RecordFree(dummy1);
    RecordFree(dummy2);
    return l;
}

/**
 * Searches  for all records with the given departure airport and day of
 * week (0 to 6), and returns them all in a list in increasing order  of
 * (hour, min, flight number).
 * Returns an empty list if there are no such records.
 * The  records  in the returned list should not be freed, but it is the
 * caller's responsibility to free the list itself.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
List     DbFindByDepartureAirportDay(FlightDb db, char *departureAirport,
                                     int day) {
	Record dummy1 = RecordNew("", departureAirport, "", day, 0, 0, 0);
	Record dummy2 = RecordNew(LARGEST_AIRPORT_STR, departureAirport, "", day, 23, 59, 0);

    List l = TreeSearchBetween(db->byDepartureAirportDay, dummy1, dummy2);
    RecordFree(dummy1);
    RecordFree(dummy2);
    return l;
}


/**
 * Searches  for  all  records  between  (day1, hour1, min1)  and (day2,
 * hour2, min2), and returns them all in a list in increasing  order  of
 * (day, hour, min, flight number).
 * Returns an empty list if there are no such records.
 * The  records  in the returned list should not be freed, but it is the
 * caller's responsibility to free the list itself.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
List     DbFindBetweenTimes(FlightDb db, 
                            int day1, int hour1, int min1, 
                            int day2, int hour2, int min2) {
	Record dummy1 = RecordNew("", "", "", day1, hour1, min1, 0);
	Record dummy2 = RecordNew(LARGEST_AIRPORT_STR, "", "", day2, hour2, min2, 0);

    List l = TreeSearchBetween(db->byTime, dummy1, dummy2);
    RecordFree(dummy1);
    RecordFree(dummy2);
    return l;
}

/**
 * Searches  for  and  returns  the  earliest next flight from the given
 * departure airport, on or after the given (day, hour, min).
 * The returned record must not be freed or modified. 
 * The time complexity of this function must be O(log n).
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
Record   DbFindNextFlight(FlightDb db, char *departureAirport, 
                          int day, int hour, int min) {
	Record r = RecordNew("", departureAirport, "", day, hour, min, 0);

	Record nextFlight = TreeNext(db->byDepartureAirportDay,r);
	RecordFree(r);
	
	//before we return the flight, we need to check that it is from the same airport
	if (nextFlight != NULL && strcmp(RecordGetDepartureAirport(nextFlight), departureAirport) == 0){
		return nextFlight;
	}
	else {
		//If no more flight this week, search for flight next week
		Record r2 = RecordNew("", departureAirport, "", 0, 0, 0, 0);
		Record nextWeekFlight = TreeNext(db->byDepartureAirportDay,r2);
		RecordFree(r2);
		if (nextWeekFlight != NULL && strcmp(RecordGetDepartureAirport(nextWeekFlight), departureAirport) == 0){
			return nextWeekFlight;
		}
		else {
			//if we still cant find a flight from the same airport, no planes depart from this airport
			return NULL;
		}
	}
}

