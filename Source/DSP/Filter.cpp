/*
  ==============================================================================
    filter.cpp
    Created: 19 Jan 2022 12:35:36pm
    Author:  Dylan
  ==============================================================================
*/

#include "filter.h"

void Filter::prepareToPlay(int numChannels, float sampleRate)
{
    updateSampleRate(sampleRate);
    state.resize(numChannels);
    reset();
}


void Filter::reset()
{
    avg = 0.0f;
    cutoffFrequency = 300.0f;
    updateCutoff();
}

void Filter::processSample(float& sample)
{
    float input = (sample - avg) * cutoffCoeff;
    sample = input + avg;
    avg = sample + input;
}

float Filter::advanceFilter(float sample)
{
    float input = (sample - avg) * cutoffCoeff;
    float output = input + avg;
    avg = output + input;
    return output;
}

void Filter::processSample(float& sample, int channel)
{
    float input = (sample - state[channel]) * cutoffCoeff;
    sample = input + state[channel];
    state[channel] = sample + input;
}

void Filter::updateSampleRate(float sampleRate)
{
    sampleTime = 1.0f / sampleRate;
    updateCutoff();
}

void Filter::updateCutoff(float frequency)
{
    float nyquistFreq = 1 / (sampleTime *  2.0f);
    //limits cutoff to slightly below nyqyuist frequency
    cutoffFrequency = std::min(frequency, nyquistFreq - 50.0f);
    updateCutoff();

    cutoffFrequency = frequency;
    updateCutoff();
    void updateTimeConstant(float seconds);
}

void Filter::updateTimeConstant(float seconds)
{
    cutoffFrequency = 1.0f / (twoPi * seconds);
    updateCutoff();
}

void Filter::processBlock(float* samplePointer, int numSamples, int channel)
{
        for (int sample = 0; sample < numSamples; ++sample)
        {
            processSample(samplePointer[sample], channel);
        }
}

void Filter::updateCutoff()
{
    //i am praying that compiling takes care of this. There is a lot of cleanup that could happen here
    //but then the variables would have a lot less meaning
    float wd = twoPi * cutoffFrequency;
    float prewarpCutoff = (2.0f / sampleTime) * tanf(wd * sampleTime / 2.0f);
    float g = prewarpCutoff * sampleTime / 2.0f;
    cutoffCoeff = g / (1.0f + g);
}