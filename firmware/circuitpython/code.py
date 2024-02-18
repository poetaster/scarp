# picostepseq_code.py -- picostepseq MIDI step sequencer
# 6 Aug 2022 - @todbot / Tod Kurt
# Part of picostepseq: https://github.com/todbot/picostepseq/
#
# User interface:
#
# - Play / Pause -- tap encoder button
# - Transpose sequence -- turn encoder knob
# - Change tempo -- push + turn encoder knob
#
# - Mute / Unmute steps -- tap corresponding step key (while playing)
# - Play notes -- tap corresponding step key (while paused)
# - Change step pitch -- hold step key, turn encoder knob
# - Change step gate -- hold step key, push + turn encoder knob
# - Change sequence -- push encoder, tap step key 1-8, release encoder
# - Save sequence -- push encoder, hold step key 1-8 for 2 secs, release encoder

#
# Or from the device point-of-view:
#
# "Encocder-first" actions:
# - Tap encoder to toggle play / pause
# - Turn encoder to change tranpose
# - Push + turn encoder to change transpose
# - Push encoder + push step key to load sequence 1-8
# - Hold encoder + hold step key > 1 sec to save sequence 1-8
# - Sequences saved to disk on pause
# "Step key-first" actions:
# - Tap step button to enable/disable from sequence
# - Hold step button + turn encoder to change note
# - Hold step button + push encoder + turn encoder to change gate length
#

# built in libraries
import board, analogio, keypad
import gc
import json
import usb_midi

# added audio from arpy
import audiopwmio, audiomixer, synthio
import ulab.numpy as np
num_voices = 1       # how many voices for each note
lpf_basef = 2500     # filter lowest frequency
lpf_resonance = 1.5  # filter q

knobA = analogio.AnalogIn(board.A0)
knobB = analogio.AnalogIn(board.A1)

audio = audiopwmio.PWMAudioOut(board.GP22)  # RX pin on QTPY RP2040

mixer = audiomixer.Mixer(channel_count=1, sample_rate=22050, buffer_size=2048)
synth = synthio.Synthesizer(channel_count=1, sample_rate=22050)
audio.play(mixer)
mixer.voice[0].play(synth)
mixer.voice[0].level = 0.8

# our oscillator waveform, a 512 sample downward saw wave going from +/-30k
wave_saw = np.linspace(30000, -30000, num=512, dtype=np.int16)  # max is +/-32k but gives us headroom
amp_env = synthio.Envelope(attack_level=1, sustain_level=1, release_time=0.5)

voices=[]  # holds our currently sounding voices ('Notes' in synthio speak)

knobfilter = 0.75
knobAval = knobA.value
knobBval = knobB.value
led_pins = (board.GP1, board.GP3, board.GP5, board.GP7,
            board.GP9, board.GP11, board.GP13, board.GP15)

# local libraries in CIRCUITPY
import winterbloom_smolmidi as smolmidi
from sequencer import StepSequencer, ticks_ms

if 'macropad' in board.board_id:
    from sequencer_display_macropad import SequencerDisplayMacroPad as SequencerDisplay
    from sequencer_hardware_macropad import Hardware
else:
    import displayio
    displayio.release_displays() # can we put this in sequencer_hardware?
    from sequencer_display import SequencerDisplay
    from sequencer_hardware import Hardware

do_usb_midi = True
do_serial_midi = True

playdebug = False

midi_chan = 1
base_note = 60  #  60 = C4, 48 = C3
num_steps = 8
tempo = 100
gate_default = 8    # ranges 0-15

# array of sequences used by Sequencer (which only knows about one sequence)
sequences = [ [(None)] * num_steps ] * num_steps  # pre-fill arrays for easy use later


usb_out = usb_midi.ports[1]
usb_in = usb_midi.ports[0]

usb_midi_in = smolmidi.MidiIn(usb_in)


midiclk_cnt = 0
midiclk_last_millis = 0
def midi_receive():
    """Handle MIDI Clock and Start/Stop"""
    global midiclk_cnt, midiclk_last_millis

    msg = usb_midi_in.receive()

    if not msg: return

    if msg.type == smolmidi.START:
        print("MIDI START")
        seqr.play()
        seqr_display.update_ui_playing()

    elif msg.type == smolmidi.STOP:
        print("MIDI STOP")
        seqr.stop()
        seqr_display.update_ui_playing()

    elif msg.type == smolmidi.CLOCK:
        midiclk_cnt += 1
        if midiclk_cnt % 6 == 0:  # once every 1/16th note (24 pulses per quarter note => 6 pulses per 16th note)
            now = ticks_ms()
            seqr.trigger_next(now)

            if midiclk_cnt % 24 == 0:  # once every quarter note
                beat_millis = (now - midiclk_last_millis) / 4  # beat_millis is 1/16th note time
                midiclk_last_millis = now
                seqr.beat_millis = beat_millis
                seqr_display.update_ui_bpm()
                seqr_display.update_ui_playing()


# from audio add arpy

# called by arpy to turn on a note
def note_on(n):
    print("  note on ", n )
    fo = synthio.midi_to_hz(n)
    voices.clear()  # delete any old voices
    for i in range(num_voices):
        f = fo * (1 + i*0.007)
        lpf_f = fo * 8  # a kind of key tracking
        lpf = synth.low_pass_filter( lpf_f, lpf_resonance )
        voices.append( synthio.Note( frequency=f, filter=lpf, envelope=amp_env, waveform=wave_saw) )
    synth.press(voices)
    

# called by arpy to turn off a note
def note_off(n):
    print("  note off", n)
    synth.release(voices)

# simple range mapper, like Arduino map()
def map_range(s, a1, a2, b1, b2): return  b1 + ((s - a1) * (b2 - b1) / (a2 - a1))


def play_note_on(note, vel, gate, on):  #
    """Callback for sequencer when note should be turned on"""
    if not on: return
    if playdebug: print("on :%d n:%3d v:%3d %d %d" % (note,vel, gate,on), end="\n" )
    note_on(note)
    midi_status = 0x90 | (midi_chan-1)
    midi_msg = bytearray([midi_status, note, vel])
    if do_usb_midi:
        usb_out.write( midi_msg )
    if do_serial_midi:
        hw.midi_uart.write( midi_msg )

def play_note_off(note, vel, gate, on):  #
    """Callback for sequencer when note should be turned off"""
    #if on: # FIXME: always do note off to since race condition of note muted right after playing
    if playdebug: print("off:%d n:%3d v:%3d %d %d" % (note,vel, gate,on), end="\n" )
    note_off(note)
    midi_status = 0x80 | (midi_chan-1)
    midi_msg = bytearray([midi_status, note, vel])
    if do_usb_midi:
        usb_out.write( midi_msg )
    if do_serial_midi:
        hw.midi_uart.write( midi_msg )

def sequence_load(seq_num):
    """Load a single sequence into the sequencer from RAM storage"""
    new_seq = sequences[seq_num].copy()
    seqr.steps = new_seq
    seqr.seqno = seq_num

def sequence_save(seq_num):
    """Store current sequence in sequencer to RAM storage"""
    sequences[seq_num] = seqr.steps.copy()

def sequences_read():
    """Read entire sequence set from disk into RAM"""
    global sequences
    print("READING ALL SEQUENCES")
    with open('/saved_sequences.json', 'r') as fp:
        sequences = json.load(fp)

last_write_time = ticks_ms()
def sequences_write():
    """Write  entire sequence set from RAM to disk"""
    global last_write_time
    if ticks_ms() - last_write_time < 20000: # only allow writes every 10 seconds
        print("NO WRITE: TOO SOON")
        return
    last_write_time = ticks_ms()
    print("WRITING ALL SEQUENCES")
    with open('/saved_sequences.json', 'w') as fp:
        json.dump(sequences, fp)


hw = Hardware()

seqr = StepSequencer(num_steps, tempo, play_note_on, play_note_off, playing=False)

sequences_read()

seqr_display = SequencerDisplay(seqr)
hw.display.show(seqr_display)

sequence_load(0)

# init display UI
seqr_display.update_ui_all()

# various state for UI handling
encoder_val_last = hw.encoder.position # needed to calculate encoder_delta
encoder_push_millis = 0  # when was encoder pushed, 0 == no push
encoder_delta = 0  # how much encoder was turned, 0 == no turn
step_push = -1  # which step button is being pushed, -1 == no push
step_push_millis = 0  # when was a step button pushed
step_edited = False  # was a step edited while it was held?

print("Ready.")

while True:
    gc.collect()  # just to make the timing of this consistent

    midi_receive()

    seqr.update()

    # update step LEDs
    for i in range(num_steps):
        (n,v,gate,on) = seqr.steps[ i ]
        if i == seqr.i:  
            c = True  # UI: bright red = indicate sequence position
            hw.led_set(i,True)
        elif on:         
            cmax = 20   # UI: dim red = indicate mute/unmute state
            hw.led_set(i,True)
        else:            
            c = False    # UI: off = muted
            hw.led_set(i,False)
        #c = max( hw.led_get(i) - hw.leds_fade_amount, cmax)  # nice fade
        #hw.led_set(i,True)
    hw.leds_show()

    seqr_display.update_ui_step()

    now = ticks_ms()

    # update encoder turning
    encoder_val = hw.encoder.position
    if encoder_val != encoder_val_last:
        encoder_delta = (encoder_val - encoder_val_last)
        encoder_val_last = encoder_val

    # UI: encoder push + hold step key = save sequence
    #print(encoder_push_millis, now-step_push_millis)
    if encoder_push_millis > 0 and step_push_millis > 0:
        if encoder_push_millis < step_push_millis:  # encoder pushed first
            if now - step_push_millis > 1000:
                seqr_display.update_ui_seqno(f"SAVE:{step_push}")


    # on encoder turn
    if encoder_delta:

        # UI: encoder turned and pushed while step key held == change step's gate
        if step_push > -1 and encoder_push_millis > 0:
            (n,v,gate,on) = seqr.steps[ step_push ]
            gate = min(max(gate + encoder_delta, 1), 15)
            seqr.steps[ step_push ] = (n,v,gate,on)
            step_edited = True
            seqr_display.update_ui_step( step_push, n, v, gate, on, True)

        # UI: encoder turned while step key held == change step's note
        elif step_push > -1:  # step key pressed
            (n,v,gate,on) = seqr.steps[ step_push ]
            if not seqr.playing:
                play_note_off( n, v, gate, True)  # step note preview note off

            n = min(max(n + encoder_delta, 1), 127)

            if not seqr.playing:
                play_note_on( n, v, gate, True )  # step note preview note on

            seqr.steps[ step_push ] = (n,v,gate,on)
            step_edited = True
            seqr_display.update_ui_step( step_push, n, v, gate, on, True)

        # UI: encoder turned while encoder pushed == change tempo
        elif encoder_push_millis > 0:
            tempo = tempo + encoder_delta
            seqr.set_tempo(tempo)
            seqr_display.update_ui_bpm()

        # UI: encoder turned without any modifiers == change transpose
        else:
            seqr.transpose = min(max(seqr.transpose + encoder_delta, -36), 36)
            seqr_display.update_ui_transpose()
        encoder_delta = 0  # we used up encoder delta

    # on encoder push
    encsw = hw.encoder_switch.events.get()
    if encsw:
        if encsw.pressed:
            #print("encoder_switch: press")
            encoder_push_millis = now  # save when we pushed encoder

        if encsw.released:
            #print("encoder_switch: release")
            if step_push == -1 and encoder_delta == 0:  # step key is not pressed and no turn
                # UI: encoder tap, with no key == play/pause
                if ticks_ms() - encoder_push_millis < 300:
                    seqr.toggle_play_pause()
                    seqr_display.update_ui_playing()
                    if not seqr.playing:
                        sequences_write()
                # UI encoder hold with no key == STOP and reset playhead to 0
                # FIXME: broken. doesn't re-start at 0 properly
                # elif ticks_diff( ticks_ms(), encoder_push_millis) > 1000:
                #     seqr.stop()
                #     seqr_display.update_ui_all()
            else:  # step key is pressed
                pass
            encoder_push_millis = 0  # say we are done with encoder, on key release


    # on step key push
    key = hw.keys.events.get()
    if key:
        try:
            # record which step key is pushed for other UI modifiers
            # .index() throws the ValueError, thus the try/except
            step_push = hw.step_to_key_pos.index(key.key_number) # map key pos back to step num
            (n,v,gate,on) = seqr.steps[step_push]

            if key.pressed:
                #print("+ press", key.key_number, "step_push:",step_push)
                step_push_millis = ticks_ms()

                # encoder push + key push = load/save sequence ## FIXME need to clean this up
                if encoder_push_millis > 0:
                    pass
                else:
                    seqr_display.update_ui_step( step_push, n, v, gate, on, True)
                    if seqr.playing:
                        pass
                    # UI: if not playing, step keys == play their pitches
                    else:
                        play_note_on( n, v, gate, True )  # step note preview note on

            elif key.released:
                #print("- release", key.key_number, step_push)

                if encoder_push_millis > 0:   # UI load /save sequence mode
                    # UI: encoder push + hold step key = save sequence
                    if now - step_push_millis > 1000:
                        sequence_save( step_push )
                        seqr_display.update_ui_seqno()
                        seqr_display.update_ui_step()
                    # UI: encoder push + tap step key = load sequence
                    else:
                        sequence_load( step_push )
                        seqr_display.update_ui_seqno()
                        if not seqr.playing:
                            seqr_display.update_ui_steps() # causes too much lag when playing
                        (n,v,gate,on) = seqr.steps[step_push]
                else:
                    if seqr.playing:
                        if step_edited:
                            pass
                        else:
                            # UI: if playing, step keys == toggles enable (must be on relase)
                            on = not on
                            seqr.steps[step_push] = (n, v, gate, on)
                    else:
                        # UI: if not playing, step key == play their pitches
                        (n,v,gate,on) = seqr.steps[step_push]
                        play_note_off( n, v, gate, True )   # step note preview note on

                seqr_display.update_ui_step( step_push, n, v, gate, on, False)
                step_push = -1  # say we are done with key
                step_push_millis = 0 # say we're done with key push
                step_edited = False  # done editing  # FIXME we need all these vars? I think so

        except ValueError:  # undefined macropad key was pressed, ignore
            pass

#    emillis = ticks_ms() - now
#    if emillis > 2:
#        print("emillis:",emillis, ticks_ms())
