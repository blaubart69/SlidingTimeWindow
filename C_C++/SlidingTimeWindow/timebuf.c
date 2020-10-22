#include <string.h>

#define MAX_ENTRIES 1024

typedef struct _TIMEBUFFER {
    long int range;
    long int now;
    long int ticks[2];
    int      front, back; // index into ts
    int      f_active;    // front is active (1) or not (0)
    int      b_active;    // same for back
    long int ts[MAX_ENTRIES];
} TIMEBUFFER;

int after    (const long int a, const long int b);

void tb_init (TIMEBUFFER* _tb, long int range, long int timestamp, int startwith);
void tb_range(TIMEBUFFER* _tb, long int range);
void tb_set  (TIMEBUFFER* _tb, long int timestamp, int what, long int* onTicks, long int* offTicks);

//-------------------------------------------------------------------------------------------------
int after(const long int a, const long int b) {
//-------------------------------------------------------------------------------------------------

    return ((a - b) > 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------------
void tb_init(TIMEBUFFER* _tb, long int range, long int timestamp, int startwith) {
    //-------------------------------------------------------------------------------------------------

    memset(&_tb, 0, sizeof(_tb));
    _tb->range = range;
    _tb->ts[0] = _tb->now = timestamp;
    _tb->f_active = startwith;
    _tb->b_active = startwith;
}

//-------------------------------------------------------------------------------------------------
void tb_range(TIMEBUFFER* _tb, long int range) {
    //-------------------------------------------------------------------------------------------------

    _tb->range = range;
}

//-------------------------------------------------------------------------------------------------
void tb_set(TIMEBUFFER* _tb, long int timestamp, int onEvent, long int* onTicks, long int* offTicks) {
    //-------------------------------------------------------------------------------------------------

        // check if full

    long int delta = timestamp - _tb->now, back_ts;

    // check if delta >= 0

    if (_tb->f_active == onEvent) {

        _tb->ticks[onEvent] += delta;
        _tb->now = timestamp;

    }
    else { // next entry

        _tb->ticks[1 - onEvent] += delta;
        _tb->front = (_tb->front + 1) % MAX_ENTRIES;
        _tb->ts[_tb->front] = _tb->now = timestamp;
        _tb->f_active = onEvent;

    } /* endif */

    // now the processing for the back

    timestamp -= _tb->range;

    back_ts = _tb->ts[_tb->back];
    onEvent = _tb->b_active;

    if (after(timestamp, back_ts)) {

        for (;;) {

            long int next_ts = _tb->ts[(_tb->back + 1) % MAX_ENTRIES];

            _tb->ts[_tb->back] = 0; // debug only

            if (after(timestamp, next_ts)) {

                _tb->ticks[onEvent] -= next_ts - back_ts;
                _tb->back = (_tb->back + 1) % MAX_ENTRIES;
                onEvent = 1 - onEvent;
                back_ts = next_ts;

            }
            else {

                _tb->ticks[onEvent] -= timestamp - back_ts;
                _tb->ts[_tb->back] = timestamp;
                _tb->b_active = onEvent;

                break;

            } /* endif */

        } /* endfor */

    } /* endif */

    *onTicks  = _tb->ticks[1];
    *offTicks = _tb->ticks[0];
}

//=================================================================================================
int main(int argc, char* argv[]) {
//=================================================================================================

    TIMEBUFFER _tb;

    long int on, off, t;

    tb_init(&_tb, 1000, 0, 0);

    //    tb_set(  1111, 1, &on, &off );

    tb_set(&_tb, 111, 1, &on, &off);
    tb_set(&_tb, 112, 0, &on, &off);
    tb_set(&_tb, 113, 1, &on, &off);
    tb_set(&_tb, 115, 0, &on, &off);
    tb_set(&_tb, 222, 0, &on, &off);
    tb_set(&_tb, 1112, 1, &on, &off);
    tb_set(&_tb, 1114, 1, &on, &off);
    tb_set(&_tb, 1115, 0, &on, &off);
    tb_set(&_tb, 1116, 1, &on, &off);
    tb_set(&_tb, 1117, 0, &on, &off);
    tb_set(&_tb, 1118, 1, &on, &off);
    tb_set(&_tb, 2222, 0, &on, &off);
    tb_set(&_tb, 2232, 1, &on, &off);
    tb_set(&_tb, 2234, 0, &on, &off);

    tb_init(&_tb, 100, 23443, 0);

    for (t = 23456; t < 99999; t += 26) {
        tb_set(&_tb, t, 1, &on, &off);
        tb_set(&_tb, t + 13, 0, &on, &off);
        if (t == 24678) {
            tb_range(&_tb, 200);
        }
    }

    return 0;
}

// -=EOF=-

