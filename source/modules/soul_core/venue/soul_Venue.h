/*
    _____ _____ _____ __
   |   __|     |  |  |  |      The SOUL language
   |__   |  |  |  |  |  |__    Copyright (c) 2019 - ROLI Ltd.
   |_____|_____|_____|_____|

   The code in this file is provided under the terms of the ISC license:

   Permission to use, copy, modify, and/or distribute this software for any purpose
   with or without fee is hereby granted, provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
   NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
   IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

namespace soul
{

//==============================================================================
/**
    Abstract base class for a "venue" which hosts sessions.

    A venue has a similar overall API to a performer - they both load, link and
    run programs. But while a performer is synchronous and just crunches the raw
    numbers, a venue runs asynchronously and will use a performer internally to
    play the audio through some kind of real device, either local or remote.
*/
class Venue
{
public:
    Venue() {}

    /** Destructor */
    virtual ~Venue() {}

    //==============================================================================
    /**
    */
    class Session
    {
    public:
        virtual ~Session() {}

        /** Loads a set of fully-resolved modules.
            After load() has returned successfully, the getInputEndpoints() and
            getOutputEndpoints() methods become available, so you can query and connect
            them to data sources before calling link().
        */
        virtual bool load (CompileMessageList&, const Program&) = 0;

        /** Returns a list of available inputs in the currently-loaded program.
            This can only be called after a successful load() method.
        */
        virtual std::vector<InputEndpoint::Ptr> getInputEndpoints() = 0;

        /** Returns a list of available outputs in the currently-loaded program.
            This can only be called after a successful load() method.
        */
        virtual std::vector<OutputEndpoint::Ptr> getOutputEndpoints() = 0;

        /** When a program has been loaded and its endpoints have had suitable data
            sources attached, call link() to finish the process of preparing the program
            to be run.
            If this returns true, then start() can be called to begin playback.
        */
        virtual bool link (CompileMessageList&, const LinkOptions&) = 0;

        /** Instructs the venue to begin playback.
            If no program is linked, this will fail and return false.
        */
        virtual bool start() = 0;

        /** Returns true if a program is linked and playing.
            @see getStatus
        */
        virtual bool isRunning() = 0;

        /** Instructs the venue to stop playback. */
        virtual void stop() = 0;

        /** Instructs the venue to stop playback, and to unload the current program. */
        virtual void unload() = 0;

        /**
        */
        enum class State
        {
            empty,
            loaded,
            linked,
            running
        };

        /** Contains various indicators of what the venue is currently doing.
            @see getStatus
        */
        struct Status
        {
            State state;
            float cpu;
            uint32_t xruns;
            double sampleRate;
            uint32_t blockSize;
        };

        /** Returns the venue's current status. */
        virtual Status getStatus() = 0;

        /** A callback function to indicate that the venue's state has changed.
            @see setStateChangeCallback
        */
        using StateChangeCallbackFn = std::function<void(State)>;

        /** Allows the client code to attach a lambda to be called back whenever the
            current state changes.
        */
        virtual void setStateChangeCallback (StateChangeCallbackFn) = 0;
    };

    //==============================================================================
    /** Creates and returns a new session for this venue. */
    virtual std::unique_ptr<Session> createSession() = 0;

    virtual std::vector<EndpointDetails> getSourceEndpoints() = 0;
    virtual std::vector<EndpointDetails> getSinkEndpoints() = 0;

    virtual bool connectSessionInputEndpoint  (Session&, soul::InputEndpoint&,  EndpointID venueSourceID) = 0;
    virtual bool connectSessionOutputEndpoint (Session&, soul::OutputEndpoint&, EndpointID venueSinkID) = 0;
};

/// Create a standard threaded venue where a separate render thread renders the performer
std::unique_ptr<Venue> createThreadedVenue (std::unique_ptr<PerformerFactory> performerFactory);


} // namespace soul
