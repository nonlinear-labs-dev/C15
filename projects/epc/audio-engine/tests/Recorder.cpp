#include <catch.hpp>

#include <recorder/FlacFrameStorage.h>
#include <recorder/FlacDecoder.h>
#include <recorder/FlacEncoder.h>
#include <recorder/Recorder.h>
#include <recorder/RecorderInput.h>
#include <recorder/RecorderOutput.h>

#include <glibmm.h>

#include <algorithm>
#include <thread>

TEST_CASE("FlacDecoder")
{
  FlacFrameStorage storage(100000);
  FlacEncoder enc(48000, [&](auto frame, auto header) { storage.push(std::move(frame), header); });

  auto numFrames = 48000;
  SampleFrame buf[numFrames];
  std::fill(buf, buf + numFrames, SampleFrame { 0.0f, 0.0f });

  buf[0].left = 1.0f;
  buf[1].left = 0.5f;
  buf[2].left = 0.25f;
  buf[3].left = 0.125f;

  enc.push(buf, numFrames);

  REQUIRE(storage.getFrames().size() > 5);

  FlacDecoder dec(&storage, 3, 10);

  SampleFrame out[numFrames];
  dec.popAudio(out, numFrames);

  REQUIRE(out[0].left == Approx(1.0f));
  REQUIRE(out[1].left == Approx(0.5f));
  REQUIRE(out[2].left == Approx(0.25f));
  REQUIRE(out[3].left == Approx(0.125f));
}

TEST_CASE("FlacDecoder In=Out")
{
  auto numFrames = 4096;

  FlacFrameStorage storage(100000);
  FlacEncoder enc(48000, [&](auto frame, auto header) { storage.push(std::move(frame), header); });

  SampleFrame in[numFrames + 1];
  SampleFrame out[numFrames];

  for(int i = 0; i < numFrames; i++)
  {
    in[i].left = g_random_double_range(-1, 1);
    in[i].right = g_random_double_range(-1, 1);
  }

  enc.push(in, numFrames + 1);

  FlacDecoder dec(&storage, 0, std::numeric_limits<FrameId>::max());
  auto res = dec.popAudio(out, numFrames);

  REQUIRE(res == 4096);

  for(int i = 0; i < numFrames; i++)
  {
    REQUIRE(Approx(in[i].left) == out[i].left);
    REQUIRE(Approx(in[i].right) == out[i].right);
  }
}

TEST_CASE("Recorder Ring")
{
  RingBuffer<int> ring(50);

  int inVal = 0;
  int outVal = 0;

  for(auto i = 0; i < 1000; i++)
  {
    auto numIn = g_random_int_range(1, ring.getFreeSpace());
    int in[numIn];
    std::generate(in, in + numIn, [&] { return inVal++; });
    ring.push(in, numIn);

    auto numOut = g_random_int_range(1, ring.avail());
    int out[numOut];
    numOut = ring.pop(out, numOut);

    for(auto k = 0; k < numOut; k++)
    {
      if(out[k] != outVal)
        G_BREAKPOINT();

      REQUIRE(out[k] == outVal);
      outVal++;
    }
  }
}

TEST_CASE("Recorder InOut")
{
  auto sr = 44100;
  auto recordLength = 10;  // seconds
  auto numFrames = sr * recordLength;
  auto chunkSize = 1000;
  auto numChunks = numFrames / chunkSize;

  Recorder r(sr);

  SampleFrame in[numFrames];

  for(auto i = 0; i < numFrames; i++)
  {
    in[i].left = g_random_double_range(-1, 1);
    in[i].right = g_random_double_range(-1, 1);
  }

  auto inWalker = in;
  for(auto i = 0; i < numChunks; i++)
  {
    r.process(inWalker, chunkSize);
    r.getInput()->TEST_waitForSettling();
    inWalker += chunkSize;
  }

  r.getInput()->TEST_waitForSettling();

  r.getInput()->togglePause();
  auto first = r.getStorage()->getFrames().front()->id;
  r.getOutput()->setPlayPos(first);
  r.getOutput()->start();

  REQUIRE(first == 3);

  SampleFrame out[numFrames];
  std::fill(out, out + numFrames, SampleFrame { 0, 0 });

  auto outWalker = out;

  // don't be exact with the number of chunks here, because a chunk is only commited
  // at boundaries of 4096, so it may happen, that there are less chunks popped then pushed
  for(auto i = 0; i < numChunks / 2; i++)
  {
    r.getOutput()->TEST_waitForBuffersFilled(chunkSize);
    r.process(outWalker, chunkSize);
    outWalker += chunkSize;
  }

  // don't be exact with the number of frames here, because a frames
  // can only be generated by 'fuill' frames at frame boundaries
  for(auto i = 0; i < numFrames / 4; i++)
  {
    REQUIRE(Approx(in[i].left) == out[i].left);
    REQUIRE(Approx(in[i].right) == out[i].right);
  }
}