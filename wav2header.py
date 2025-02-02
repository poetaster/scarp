#!/usr/bin/python
#////////////////////////////////////////////////////////////////////////////////
#//                                                                            //
#//                               wav2Header.py                                //
#//                             -PYTHON SCRIPT-                                //
#//                      Written By:  Richard Wardlow                          //
#//                                                                            //
#//                                                                            //
#//   Summary:  Convert 8-bit mono wav to .h header file for MCU               //
#//                                                                            //
#//                                                                            //
#//                                                                            //
#////////////////////////////////////////////////////////////////////////////////

#/////////////////////////////
#// IMPORT MODULES HERE     //
#///////////////////////////// 
import pyaudio
import wave
import sys, os
import numpy
from optparse import OptionParser


#///////////
#// NOTES //
#///////////


#////////////////////////////////////
#// GLOBAL VARIABLES DECLARED HERE //
#////////////////////////////////////
VERSION = '1.0.0'

#////////////////////////////////////
#// GLOBAL FUNCTIONS DECLARED HERE //
#////////////////////////////////////
def addCommandLineOptions(parser):
    parser.add_option(
            '-f', '--file',
            dest='wavFile',
            default=None,
            help='Specify Wav file for conversion')
    parser.add_option(
            '-q', '--quality',
            dest='quality',
            default=2,
            help='Specify quality conversion 1=High, 2=Med, 3=Low')
    parser.add_option(
            '-s', '--sample',
            dest='sample',
            default=None,
            help='Optional sample override for more specific conversion quality')
    parser.add_option(
            '-o', '--output',
            dest='headerFile',
            default=None,
            help='Specify output header file')
    parser.add_option(
            '-l', '--length',
            dest='length',
            default=None,
            help='Optional Length Limit in bytes to keep the header file from being to big')

def wav2header(options):
    s = None
    print ( " Wav To Header File Conversion\n " )
    if options.wavFile == None:
            print ('Need to specify a wav file for conversion')
            print ('--help for help')
            sys.exit(0)

    if options.headerFile == None:
            print ('Need to specify a header output file')
            print ('--help for help')
            sys.exit(0)

    if int(options.quality) < 1 or int(options.quality) > 3:
            print ('Quality setting must be either 1=High, 2=Med, or 3=Low')
            print ('--help for help')
            sys.exit(0)


    wf = wave.open(options.wavFile, 'rb')
    cf = open(options.headerFile, 'w')
    if options.sample == None:
            if int(options.quality) == 1:
                    sfps = 6400
            elif int(options.quality) == 2:
                    sfps = 4800
            elif int(options.quality) == 3:
                    sfps = 3200
    else:
            sfps = int(options.sample)

    print ('Converting wav to header....')
    srate = wf.getframerate()
    smrate = int(srate / sfps)
    ws = wf.getnframes()
    data = wf.readframes(1024)
    wavsamp = int(ws / smrate) + 2
    data2 = numpy.arange(0, wavsamp)
    x = 0
    for x in range(0, wavsamp):
            data2[x] = 0

    x = 0
    y = smrate
    tmp = 0
    tmp2 = 0.0

    while data != '':
        for i in data:
            if y == smrate:
                tmp = ord(chr(i))
                 
                data2[x] = tmp
                x = x + 1
                y = 0
            y = y + 1
        data = wf.readframes(1024)
		
    xcnt = 0;
    delayTimer = int(360.0 - (((sfps / 1600.0) - 1.0) * 90.0))
    cf.write('static const unsigned short envelope_table[] = {  //Use Approx. Delay Time of: ' + str(delayTimer) + ' us @ 64Mhz\n')
    if options.length != None:
        if x > int(options.length):
            x = int(options.length)
            print ('Sample length is longer than length limit...Truncating sample to stay within limit.')

    for y in range(0,x):
        tmp2 = data2[y]
        if y == 0:
            cf.write('%i' % (tmp2))
        else:
            cf.write(',%i' % (tmp2))
        xcnt += 1
        if xcnt > 15:
            cf.write('\n')
            xcnt = 0

    cf.write('};\n')

    cf.close()
    print ('Header Bytes: ' + str(x))
    print ('Use a delay time of approx. ' + str(delayTimer) + ' us @ 64Mhz')
    print ('Conversion complete!')
    


if __name__ == '__main__':
    parser = OptionParser()
    addCommandLineOptions(parser)
    options, args = parser.parse_args()
    result = wav2header(options)
    sys.exit(result)
	

