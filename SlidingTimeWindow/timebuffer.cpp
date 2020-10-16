class Timespan
{

public:

    long int ticks;

    Timespan(long int duration)
    {
        this->ticks = duration;
    }

    void Add(Timespan other)
    {
        ticks += other.ticks;
    }
    void Sub(Timespan other)
    {
        ticks -= other.ticks;
    }
};

class Timestamp
{
private:
    long int ticks;

public:
    Timestamp(long int pointInTime) : ticks(pointInTime)
    {
    }
    Timestamp() : ticks(0)
    {
    }

    bool after(const Timestamp other) const
    {
        return (ticks - other.ticks) > 0;
    }

    Timespan Sub(const Timestamp other) const
    {
        return Timespan(ticks - other.ticks);
    }

    Timestamp SubstractTimespan(Timespan span) const
    {
        return Timestamp(ticks - span.ticks);
    }
};

#define MAX_ENTRIES 1024

class TimeBuffer
{
public:
    TimeBuffer(long int range, long int timestamp, int startwith)
        : range(range), now(timestamp), ticks{ 0, 0 }, frontIdx(0), backIdx(0), f_active(startwith), b_active(startwith)
    {
        ts[0] = timestamp;
    }
    void set_range(long int newRange)
    {
        this->range = newRange;
    }
    void set(const Timestamp timestamp, int onEvent, Timespan* onTicks, Timespan* offTicks);

private:
    Timespan range;
    Timestamp now;
    Timespan ticks[2];
    int      frontIdx, backIdx; // index into ts
    int      f_active;          // front is active (1) or not (0)
    int      b_active;          // same for back
    Timestamp ts[MAX_ENTRIES];
};

void TimeBuffer::set(const Timestamp timestamp, int onEvent, Timespan* onTicks, Timespan* offTicks)
{
    // check if full

    const Timespan delta = timestamp.Sub(now);

    // check if delta >= 0

    if (f_active == onEvent) {

        ticks[onEvent].Add(delta);
        now = timestamp;

    }
    else { // next entry

        ticks[1 - onEvent].Add(delta);
        frontIdx = (frontIdx + 1) % MAX_ENTRIES;
        ts[frontIdx] = now = timestamp;
        f_active = onEvent;

    }

    // now the processing for the back

    const Timestamp oldest_ts_inRange = timestamp.SubstractTimespan(range);   //timestamp -= range;
    Timestamp back_ts = ts[backIdx];                                    //back_ts = _tb->ts[_tb->back];
    onEvent = b_active;
    
    if ( back_ts.after(oldest_ts_inRange) ) {                           //if (after(timestamp, back_ts)) {
    
        for (;;) {

            const Timestamp next_ts = ts[(backIdx + 1) % MAX_ENTRIES];  //long int next_ts = _tb->ts[(_tb->back + 1) % MAX_ENTRIES];

            ts[backIdx] = 0;                                            //_tb->ts[_tb->back] = 0; // debug only

            if ( next_ts.after(oldest_ts_inRange) ) {                   //if (after(oldest, next_ts)) {

                ticks[onEvent].Sub( next_ts.Sub(back_ts) );             //_tb->ticks[onEvent] -= next_ts - back_ts;
                
                backIdx = (backIdx + 1) % MAX_ENTRIES;
                onEvent = 1 - onEvent;
                back_ts = next_ts;

            }
            else {

                ticks[onEvent].Sub( oldest_ts_inRange.Sub(back_ts) );   //_tb->ticks[onEvent] -= oldest - back_ts;
                
                ts[backIdx] = oldest_ts_inRange;
                b_active = onEvent;

                break;

            }
        }
    }

    *onTicks  = ticks[1];
    *offTicks = ticks[0];
}

/*

*/


// -=EOF=-

