# sequencer.py -- picostepseq sequencer object
# 6 Aug 2022 - @todbot / Tod Kurt
# Part of picostepseq : https://github.com/todbot/picostepseq/

# let's get a millis function
try:
     from supervisor import ticks_ms  # thank you dhalbert
except (ImportError,NameError,NotImplementedError):
     from time import monotonic_ns as _monotonic_ns  # assume monotonic_ns() exists else we are lame
     def ticks_ms(): return _monotonic_ns() // 1_000_000  # stolen from adafruit_ticks


def ticks_diff(t1,t2): return t1-t2

note_names = ("C","C#","D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B")

# maybe use this someday
# class Step:
#     on = True
#     # def __init__(self, note=0, vel=100, gate=8, on=True):
#     #     self.note = note
#     #     self.vel = vel
#     #     self.gate = gate # 1-16
#     #     self.on = on

class StepSequencer:
    def __init__(self, step_count, tempo, on_func, off_func, playing=False, seqno=0):
        self.ext_trigger = False  # midi clocked or not
        self.steps_per_beat = 4  # 16th note
        self.step_count = step_count
        self.i = 0  # where in the sequence we currently are
        self.steps = [ (0,100,8,True) ] * step_count  # list of step "objects", i.e. tuple (note, vel, gate, on)
        self.on_func = on_func    # callback to invoke when 'note on' should be sent
        self.off_func = off_func  # callback to invoke when 'note off' should be sent
        self.set_tempo(tempo)
        self.last_beat_millis = ticks_ms()  # 'tempo' in our native tongue
        self.held_gate_millis = 0  # when in the future our note off should occur
        self.held_note = (0,0,0,0)  # the current note being on, to be turned off
        self.transpose = 0
        self.playing = playing   # is sequence running or not (but use .play()/.pause())
        self.seqno = seqno # an 'id' of what sequence it's currently playing

    @property
    def tempo(self):  # really just used for display purposes
        return 60_000 // self.beat_millis // self.steps_per_beat

    def set_tempo(self, tempo):
        """Sets the internal tempo. beat_millis is 1/16th note time in milliseconds"""
        self.beat_millis = 60_000 // self.steps_per_beat // tempo
        print("seq.set_tempo: %6.2f %d" % (self.beat_millis, tempo) )

    def trigger_next(self, now):
        """Trigger next step in sequence (and thus make externally triggered)"""
        self.ext_trigger = True
        self.trigger(now, self.beat_millis)

    def trigger(self, now, delta_t):
        if not self.playing:
            return
        fudge = 0  # seems more like 3-10

        # go to next step in sequence, get new note, transpose if needed
        self.i = (self.i + 1) % self.step_count
        (note,vel,gate,on) = self.steps[self.i]
        note += self.transpose

        # turn off any pending note (should've been turned off, but this is just in case)
        if self.held_gate_millis > 0:
            print("HELD NOTE", self.notenum_to_name(self.held_note[0]), self.held_note[2],
                  now, self.held_gate_millis, delta_t, self.beat_millis)
            self.off_func( *self.held_note )  # FIXME: why is this getting held?

        # trigger new note
        self.on_func(note, vel, gate, on)

        # calculate next note timing and held note timing
        err_t = delta_t - self.beat_millis  # how much we are over
        #print("err_t:",self.i, err_t, self.beat_millis)
        self.last_beat_millis = now - err_t - fudge # adjust for our overage
        self.held_note = (note,vel,gate,on) # save for note off later
        self.held_gate_millis = now + ((self.beat_millis * gate) // 16) - err_t # gate ranges from 1-16

    def update(self):
        """Update state of sequencer. Must be called regularly in main"""

        now = ticks_ms()
        delta_t = now - self.last_beat_millis  # FIXME: better name, 'real_beat_millis'?

        # turn off note if it is done
        if self.held_gate_millis != 0 and now >= self.held_gate_millis:
            self.held_gate_millis = 0
            self.off_func( *self.held_note )

        # if time for new note, trigger it
        if delta_t >= self.beat_millis:
            if not self.ext_trigger:
                self.trigger(now, delta_t)
            else:
                # fall back to internal triggering if not externally clocked for a while
                if delta_t > self.beat_millis * 4:
                    self.ext_trigger = False
                    print("Turning EXT TRIGGER off")

    def toggle_play_pause(self):
        if self.playing:
            self.pause()
        else:
            self.play()

    def stop(self):  # FIXME: what about pending note
        self.playing = False
        self.i = 0
        self.last_beat_millis = 0

    def pause(self):
        self.playing = False

    def play(self, play=True):
        self.last_beat_millis = ticks_ms() - self.beat_millis # ensures we start on immediately
        self.playing = True

    def notenum_to_noteoct(self, notenum):
        """Return note and octave as (string,int) tuple"""
        octave = notenum // 12 - 2;
        notename = note_names[notenum % 12]
        return (notename, octave)

    # old do not use
    def notenum_to_name(self, notenum, separator=""):
        octave = notenum // 12 - 2;
        n = notenum % 12
        return note_names[n] +separator+ str(octave)
