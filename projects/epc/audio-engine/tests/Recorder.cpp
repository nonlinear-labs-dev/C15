#include <catch.hpp>

#include <recorder/FlacFrameStorage.h>
#include <recorder/FlacDecoder.h>
#include <recorder/FlacEncoder.h>

#include <glibmm.h>

#include <algorithm>

TEST_CASE("FlacDecoder")
{
  FlacFrameStorage storage(100000);
  FlacEncoder enc(48000, [&](auto frame) { storage.push(std::move(frame)); });

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
  FlacEncoder enc(48000, [&](auto frame) { storage.push(std::move(frame)); });

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