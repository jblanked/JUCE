#pragma once

#include <JuceHeader.h>
#include <array>

//=====================================================================
// Fifo Template
//=====================================================================
template <typename T>
struct Fifo
{
    void prepare(int numChannels, int numSamples)
    {
        static_assert(std::is_same_v<T, juce::AudioBuffer<float>>,
                      "Fifo only works with AudioBuffer<float>");
        for (auto &buffer : buffers)
        {
            buffer.setSize(
                numChannels,
                numSamples,
                false,      // clear everything?
                true,       // including the extra space?
                true);      // avoid reallocation
            buffer.clear(); // clear the buffer
        }
    }

    void prepare(size_t numElements)
    {
        static_assert(std::is_same_v<T, std::vector<float>>,
                      "Fifo only works with std::vector<float>");
        for (auto &buffer : buffers)
        {
            buffer.clear();                // clear the buffer
            buffer.resize(numElements, 0); // resize the buffer and fill with 0s
        }
    }

    bool push(const T &t)
    {
        auto write = fifo.write(1);
        if (write.blockSize1 > 0)
        {
            buffers[write.startIndex1] = t;
            return true;
        }
        return false;
    }

    bool pull(T &t)
    {
        auto read = fifo.read(1);
        if (read.blockSize1 > 0)
        {
            t = buffers[read.startIndex1];
            return true;
        }
        return false;
    }

    int getNumAvailableForReading() const
    {
        return fifo.getNumReady();
    }

private:
    static constexpr int Capacity = 30; // number of buffers
    std::array<T, Capacity> buffers;    // array of buffers
    juce::AbstractFifo fifo{Capacity};  // FIFO to manage the buffers
};

//=====================================================================
// Channel Enum
//=====================================================================
enum Channel
{
    Left, // 0
    Right // 1
};

//=====================================================================
// SingleChannelSampleFifo Template
//=====================================================================
template <typename BlockType>
struct SingleChannelSampleFifo
{
    SingleChannelSampleFifo(Channel ch) : channelToUse(ch)
    {
        prepared.set(false);
    }

    void update(const BlockType &buffer)
    {
        jassert(prepared.get());
        jassert(buffer.getNumChannels() > channelToUse);        // ensure the buffer is valid
        auto *channelPtr = buffer.getReadPointer(channelToUse); // get the channel data

        for (int i = 0; i < buffer.getNumSamples(); ++i) // loop through the samples
        {
            pushNextSampleIntoFifo(channelPtr[i]); // push each sample into the FIFO
        }
    }

    void prepare(int bufferSize)
    {
        prepared.set(false);  // set the prepared flag to false
        size.set(bufferSize); // set the size of the buffer

        // create the buffer
        bufferToFill.setSize(
            1,          // one channel
            bufferSize, // number of samples
            false,      // do not keep existing data
            true,       // clear extra space
            true);      // avoid reallocation

        audioBufferFifo.prepare(1, bufferSize); // prepare the FIFO for one channel
        fifoIndex = 0;                          // reset the FIFO index
        prepared.set(true);                     // mark as prepared
    }

    int getNumCompleteBuffersAvailable() const { return audioBufferFifo.getNumAvailableForReading(); }
    bool isPrepared() const { return prepared.get(); }
    int getSize() const { return size.get(); }
    bool getAudioBuffer(BlockType &buf) { return audioBufferFifo.pull(buf); }

private:
    Channel channelToUse;                // the channel to use
    int fifoIndex = 0;                   // current index in the buffer
    Fifo<BlockType> audioBufferFifo;     // FIFO for audio buffers
    BlockType bufferToFill;              // buffer to accumulate samples
    juce::Atomic<bool> prepared = false; // flag to indicate if prepared
    juce::Atomic<int> size = 0;          // size of the buffer

    void pushNextSampleIntoFifo(float sample)
    {
        if (fifoIndex == bufferToFill.getNumSamples()) // FIFO is full
        {
            auto ok = audioBufferFifo.push(bufferToFill); // push the full buffer
            juce::ignoreUnused(ok);
            fifoIndex = 0; // reset index after push
        }
        bufferToFill.setSample(0, fifoIndex, sample); // add sample to buffer
        ++fifoIndex;                                  // increment FIFO index
    }
};
