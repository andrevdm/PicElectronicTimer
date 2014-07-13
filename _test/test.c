#include <stdio.h>
#include "xc.h"
#include "seatest.h"
#include "timer.h"
#include "timerUi.h"

//
// create a test...
//
void test_demo()
{
        char *s = "hello world!";
        assert_string_equal("hello world!", s);
        assert_string_contains("hello", s);
        assert_string_doesnt_contain("goodbye", s);
        assert_string_ends_with("!", s);
        assert_string_starts_with("hell", s);
}

void test_getPgm()
{
  PgmData *pgm = getPgmData(0);
  pgm->fromH = 12;

  pgm = getPgmData(1);
  pgm = getPgmData(0);

  assert_int_equal(12,pgm->fromH);
}

void test_inPeriod()
{
  PgmData *pgm = getPgmData(2);
  pgm->status = PGM_HAS_PERIOD | PGM_AUTO;

  assert_false( pgm->isOn );

  pgm->fromH = 9;
  pgm->toH = 10;

  dt_hour = 9;
  dt_minute = 30;
  updateRelays();
  assert_true( pgm->isOn );

  dt_hour = 10;
  dt_minute = 30;
  updateRelays();
  assert_false( pgm->isOn );
}


void test_inPeriodPastMidnight()
{
  PgmData *pgm = getPgmData(2);
  pgm->status = PGM_HAS_PERIOD | PGM_AUTO;

  assert_false( pgm->isOn );

  //09:00 - 03:00
  pgm->fromH = 9;
  pgm->fromM = 0;

  pgm->toH = 3;
  pgm->toM = 0;

  dt_hour = 23;
  dt_minute = 59;
  updateRelays();
  assert_true( pgm->isOn );

  dt_hour = 00;
  dt_minute = 00;
  updateRelays();
  assert_true( pgm->isOn );

  dt_hour = 2;
  dt_minute = 59;
  updateRelays();
  assert_true( pgm->isOn );

  dt_hour = 3;
  dt_minute = 0;
  updateRelays();
  assert_false( pgm->isOn );
}

void test_forceOff()
{
  PgmData *pgm = getPgmData(2);
  pgm->status = PGM_HAS_PERIOD | PGM_FORCE_OFF;

  assert_false( pgm->isOn );

  pgm->fromH = 9;
  pgm->toH = 10;

  dt_hour = 9;
  dt_minute = 30;
  updateRelays();
  assert_false( pgm->isOn );
}

void test_forceOn()
{
  PgmData *pgm = getPgmData(2);
  pgm->status = PGM_HAS_PERIOD | PGM_FORCE_ON;

  assert_false( pgm->isOn );

  pgm->fromH = 9;
  pgm->toH = 10;

  dt_hour = 11;
  dt_minute = 30;
  updateRelays();
  assert_true( pgm->isOn );
}

void test_repStartsOnIfInPeriod()
{
  initialisePgms();

  PgmData *pgm = getPgmData(2);
  assert_true( pgm->toggleAt < 0 );
  assert_int_equal( 0, pgm->isRepOn );

  pgm->status = PGM_HAS_REP;
  pgm->everyM = 5;
  pgm->forS = 10;

  dt_hour = 10;
  dt_minute = 30;
  dt_second = 0;
  updateRelays();
  assert_true( pgm->isOn );
  //assert_int_equal( 1, RELAY3 );
}

void test_repStateToggle()
{
  printf("\r\nrepStateToggle\r\n");
  initialisePgms();

  PgmData *pgm = getPgmData(2);
  pgm->status = PGM_HAS_REP;
  pgm->everyM = 5;
  pgm->forS = 10;

  //No period - so should start on for rep period
  dt_hour = 10;
  dt_minute = 30;
  dt_second = 0;
  updateRelays();
  assert_true( pgm->isOn );

  //Still on 9/10 seconds elapsed
  dt_hour = 10;
  dt_minute = 30;
  dt_second = 9;
  updateRelays();
  assert_true( pgm->isOn );

  //rep period has elapsed - should be off
  dt_hour = 10;
  dt_minute = 30;
  dt_second = 10;
  updateRelays();
  assert_false( pgm->isOn );

  //still off - 1 second before next rep
  //NB: note that the next rep count starts *after* the last "on" period *ended*
  dt_hour = 10;
  dt_minute = 35;
  dt_second = 9;
  updateRelays();
  assert_false( pgm->isOn );

  //Back on again, 1 second after expected on
  dt_hour = 10;
  dt_minute = 35;
  dt_second = 11;
  updateRelays();
  assert_true( pgm->isOn );

  //off
  dt_hour = 10;
  dt_minute = 35;
  dt_second = 21;
  updateRelays();
  assert_false( pgm->isOn );

  //on
  dt_hour = 10;
  dt_minute = 40;
  dt_second = 21;
  updateRelays();
  assert_true( pgm->isOn );
}

void test_repStateToggleWithTimeWrap()
{
  printf("\r\nrepStateToggleWithTimeWrap\r\n");
  initialisePgms();

  PgmData *pgm = getPgmData(2);
  pgm->status = PGM_HAS_REP;
  pgm->everyM = 5;
  pgm->forS = 60;

  //No period - so should start on for rep period
  dt_hour = 23;
  dt_minute = 59;
  dt_second = 50;
  updateRelays();
  assert_true( pgm->isOn );

  //Still on - same day, so prior to time wrapping
  dt_hour = 23;
  dt_minute = 59;
  dt_second = 59;
  updateRelays();
  assert_true( pgm->isOn );

  //Still on 11/60 seconds elapsed
  dt_hour = 0;
  dt_minute = 0;
  dt_second = 49;
  updateRelays();
  assert_true( pgm->isOn );

  //rep period has elapsed - should be off
  dt_hour = 0;
  dt_minute = 0;
  dt_second = 50;
  updateRelays();
  assert_false( pgm->isOn );

  //still off - 1 second before next rep
  //NB: note that the next rep count starts *after* the last "on" period *ended*
  dt_hour = 0;
  dt_minute = 5;
  dt_second = 49;
  updateRelays();
  assert_false( pgm->isOn );

  //Back on again
  dt_hour = 0;
  dt_minute = 5;
  dt_second = 50;
  updateRelays();
  assert_true( pgm->isOn );
}

void test_endOfPeriodEndsRep()
{
  printf("\r\nendOfPeriodEndsRep\r\n\r\n");
  initialisePgms();

  PgmData *pgm = getPgmData(2);
  pgm->status = PGM_HAS_REP | PGM_HAS_PERIOD;
  pgm->fromH = 10;
  pgm->fromM = 00;
  pgm->toH = 10;
  pgm->toM = 31;
  pgm->everyM = 5;
  pgm->forS = 999;

  printf( "\r\n-starts on\r\n" );
  dt_hour = 10;
  dt_minute = 30;
  dt_second = 0;
  updateRelays();
  assert_true( pgm->isOn );

  printf( "\r\n-still in rep but out of period - so the relay should be off\r\n" );
  dt_hour = 10;
  dt_minute = 31;
  dt_second = 0;
  updateRelays();
  assert_false( pgm->isOn );

  printf( "\r\n-set rep off and toggleAt=-1 so that the next time the period is entered the rep will start\r\n" );
  assert_false( pgm->isRepOn );
  assert_int_equal( -1, pgm->toggleAt );

  printf( "\r\n-next period - rep should be on again\r\n" );
  dt_hour = 10;
  dt_minute = 30;
  dt_second = 0;
  updateRelays();
  assert_true( pgm->isOn );
}

void test_inRep()
{
  initialisePgms();

  PgmData *pgm = getPgmData(2);
  pgm->status = PGM_HAS_REP;

  dt_hour = 10;
  dt_minute = 30;
  updateRelays();
  assert_true( pgm->isOn );
}

void test_repAfterTimeChangeForward()
{
  printf("\r\nrepAfterTimeChangeForward\r\n");
  initialisePgms();

  PgmData *pgm = getPgmData(2);
  pgm->status = PGM_HAS_REP;
  pgm->everyM = 5;
  pgm->forS = 10;

  //No period - so should start on for rep period
  dt_hour = 10;
  dt_minute = 30;
  dt_second = 0;
  updateRelays();
  assert_true( pgm->isOn );

  //rep period has elapsed - should be off
  dt_hour = 10;
  dt_minute = 30;
  dt_second = 10;
  updateRelays();
  assert_false( pgm->isOn );

  //The next rep is due at 10:15. But the time was changed forward
  timeEdited = 1;
  dt_hour = 20;
  dt_minute = 35;
  dt_second = 10;
  updateRelays();
  assert_false( pgm->isOn );

  //Still off
  dt_hour = 20;
  dt_minute = 40;
  dt_second = 9;
  updateRelays();
  assert_false( pgm->isOn );

  //Should be back on after reset
  dt_hour = 20;
  dt_minute = 40;
  dt_second = 10;
  updateRelays();
  assert_true( pgm->isOn );
}

void test_repAfterTimeChangeBackward()
{
  printf("\r\nrepAfterTimeChangeBackwards\r\n");
  initialisePgms();

  PgmData *pgm = getPgmData(2);
  pgm->status = PGM_HAS_REP;
  pgm->everyM = 5;
  pgm->forS = 10;

  //No period - so should start on for rep period
  dt_hour = 10;
  dt_minute = 30;
  dt_second = 0;
  updateRelays();
  assert_true( pgm->isOn );

  //rep period has elapsed - should be off
  dt_hour = 10;
  dt_minute = 30;
  dt_second = 10;
  updateRelays();
  assert_false( pgm->isOn );

  //The next rep is due at 10:15. But the time was changed backwards
  timeEdited = 1;
  dt_hour = 8;
  dt_minute = 35;
  dt_second = 10;
  updateRelays();
  assert_false( pgm->isOn );

  dt_hour = 8;
  dt_minute = 40;
  dt_second = 9;
  updateRelays();
  assert_false( pgm->isOn );

  //Should be on again after rep reset because of edit
  dt_hour = 8;
  dt_minute = 40;
  dt_second = 10;
  updateRelays();
  assert_true( pgm->isOn );
}

//
// put the test into a fixture...
//
void test_fixture_main( void )
{
        test_fixture_start();
        run_test(test_demo);
        run_test(test_getPgm);
        run_test(test_inPeriod);
        run_test(test_inPeriodPastMidnight);
        run_test(test_forceOff);
        run_test(test_forceOn);
        run_test(test_repStartsOnIfInPeriod);
        run_test(test_repStateToggle);
        run_test(test_endOfPeriodEndsRep);
        run_test(test_repStateToggleWithTimeWrap);
        run_test(test_repAfterTimeChangeForward);
        run_test(test_repAfterTimeChangeBackward);
        test_fixture_end();
}

//
// put the fixture into a suite...
//
void all_tests( void )
{
        test_fixture_main();
}

//
// run the suite!
//
int main( int argc, char** argv )
{
        return run_tests(all_tests);
}
