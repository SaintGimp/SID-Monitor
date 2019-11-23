/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "/Users/elee/Projects/SaintGimp/SID-Monitor/code/SID-Monitor/src/SID-Monitor.ino"
#include "Particle.h"

#include "ADCDMAGen3_RK.h"

#include "FftComplex.h" // https://www.nayuki.io/page/free-small-fft-in-multiple-languages
#include <cmath> // std::abs

void setup();
void loop();
#line 8 "/Users/elee/Projects/SaintGimp/SID-Monitor/code/SID-Monitor/src/SID-Monitor.ino"
SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler;

const unsigned long UPDATE_PERIOD_MS = 20;

const size_t SAMPLE_FREQ = 200000; // Hz

const size_t SAMPLES_IN_BUFFER = 1024;

static nrf_saadc_value_t buffer[SAMPLES_IN_BUFFER * 2];

static nrf_saadc_value_t *bufferReady = 0;

std::vector< std::complex<double> > complexSamples;

const unsigned long UPDATE_PERIOD = 20;

unsigned long lastUpdate;

ADCDMAGen3 adc;

void setup() {
	// Optional, just for testing so I can see the logs below
	//waitFor(Serial.isConnected, 10000);

	// Pre-allocate an array of complex<double> samples in a vector. We reuse this for FFT.
	complexSamples.resize(SAMPLES_IN_BUFFER);

	adc.withBufferCallback([](nrf_saadc_value_t *buf, size_t size) {
		// This gets executed after each sample buffer has been read.
		// Note: This is executed in interrupt context, so beware of what you do here!

		// We just remember the buffer and handle it from loop
		bufferReady = buf;
	});

	ret_code_t err = adc
		.withSampleFreqHz(SAMPLE_FREQ)
		.withDoubleBufferSplit(SAMPLES_IN_BUFFER * 2, buffer)
		.withResolution(NRF_SAADC_RESOLUTION_12BIT)
		.withAcqTime(NRF_SAADC_ACQTIME_3US)
		.withSamplePin(A0)
		.init();

	Log.info("adc.init %lu", err);

	adc.start();

  pinMode(D7, OUTPUT); 
}

void loop() {
  if (bufferReady) {
    digitalWrite(D7, HIGH);
    int16_t *src = (int16_t *)bufferReady;
    bufferReady = 0;

    for (size_t ii = 0; ii < SAMPLES_IN_BUFFER; ii++) {
      complexSamples[ii] = (double)src[ii] - 2048;
    }

  	//Log.info("raw: %d", src[256]);

    // Run the FFT on the samples
    Fft::transformRadix2(complexSamples);
    digitalWrite(D7, LOW);

    // Results in the frequency domain are from index 0 to index (SAMPLES_IN_BUFFER / 2)
    // where index 0 is the DC component and the maximum indexex is the highest
    // frequency, which is 1/2 the sampling frequency.
    // (The top half of the buffer is the negative frequencies, which we ignore.)
    // Start at 1 to ignore the DC component
    // for (size_t ii = 1; ii < SAMPLES_IN_BUFFER / 2; ii++) {
    //   int freq = ii * SAMPLE_FREQ / SAMPLES_IN_BUFFER;
    //   double value = std::abs(complexSamples[ii]) / SAMPLES_IN_BUFFER;

    //   if (value > 10)
    //   {
    //     Log.info("[%d] %d - %f", ii, freq, value);
    //   }
    // }
  

    //Log.info("done");
  }
}