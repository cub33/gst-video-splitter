#include <gst/gst.h>

int main(int argc, char *argv[]) {
  GstElement *pipeline;
  GstElement *audio_source, *audio_tee, *video_tee, *video_queue, *audio_queue, *audio_convert, *audio_resample, *audio_sink;
  GstElement *video_queue_720, *video_queue_480, *video_queue_360, *visual, *video_convert;
  GstElement *video_scale_720, *video_scale_480, *video_scale_360, *video_sink_0, *video_sink_1, *video_sink_2, *rtmp_sink;
  GstBus *bus;
  GstMessage *msg;
  GstPad *audio_tee_audio_pad, *audio_tee_video_pad, *tee_720_video_pad, *tee_480_video_pad, *tee_360_video_pad, *tee_rtmp_video_pad;
  GstPad *queue_audio_pad, *queue_video_pad, *queue_720_video_pad, *queue_480_video_pad, *queue_360_video_pad, *queue_rtmp_video_pad;

  /* initialize GStreamer */
  gst_init (&argc, &argv);

  audio_source = gst_element_factory_make ("audiotestsrc", "main_source");
  audio_tee = gst_element_factory_make ("tee", "audio_tee");
  video_tee = gst_element_factory_make ("tee", "video_tee");
  video_queue = gst_element_factory_make ("queue", "video_queue");
  audio_queue = gst_element_factory_make ("queue", "audio_queue");
  audio_convert = gst_element_factory_make ("audioconvert", "audio_convert");
  audio_resample = gst_element_factory_make ("audioresample", "audio_resample");
  audio_sink = gst_element_factory_make ("autoaudiosink", "audio_sink");

  video_queue_720 = gst_element_factory_make ("queue", "video_queue_720");
  video_queue_480 = gst_element_factory_make ("queue", "video_queue_480");
  video_queue_360 = gst_element_factory_make ("queue", "video_queue_360");

  visual = gst_element_factory_make ("wavescope", "visual");
  video_convert = gst_element_factory_make ("videoconvert", "converter");

  video_scale_720 = gst_element_factory_make ("videoscale", "to720");
  video_scale_480 = gst_element_factory_make ("videoscale", "to480");
  video_scale_360 = gst_element_factory_make ("videoscale", "to360");

  video_sink_0 = gst_element_factory_make ("autovideosink", "video_sink_0");
  video_sink_1 = gst_element_factory_make ("autovideosink", "video_sink_1");
  video_sink_2 = gst_element_factory_make ("autovideosink", "video_sink_2");
  rtmp_sink = gst_element_factory_make ("rtmpsink", "rtmp_sink");


  pipeline = gst_pipeline_new ("main-pipeline");

  /* configure elements */
  g_object_set (audio_source, "freq", 215.0f, NULL);
  GstCaps *caps_720 = gst_caps_new_simple ("video/x-raw",
      "width", G_TYPE_INT, 1280,
      "height", G_TYPE_INT, 720,
      NULL);
  GstCaps *caps_480 = gst_caps_new_simple ("video/x-raw",
      "width", G_TYPE_INT, 848,
      "height", G_TYPE_INT, 480,
      NULL);
  GstCaps *caps_360 = gst_caps_new_simple ("video/x-raw",
      "width", G_TYPE_INT, 640,
      "height", G_TYPE_INT, 360,
      NULL);
  

  gst_bin_add_many (GST_BIN (pipeline),
    audio_source, visual, video_convert, audio_tee, video_tee,
    video_queue, audio_queue, audio_convert, audio_resample, audio_sink,
    video_queue_720, video_scale_720, video_sink_0,
    video_queue_480, video_scale_480, video_sink_1,
    video_queue_360, video_scale_360, video_sink_2,
    NULL);

  if (gst_element_link_many (audio_source, audio_tee, NULL) != TRUE ||
      gst_element_link_many (video_queue, visual, video_convert, video_tee, NULL) != TRUE ||
      gst_element_link_many (audio_queue, audio_convert, audio_resample, audio_sink, NULL) != TRUE ||
      gst_element_link (video_queue_720, video_scale_720) != TRUE || gst_element_link_filtered(video_scale_720, video_sink_0, caps_720) != TRUE ||
      gst_element_link (video_queue_480, video_scale_480) != TRUE || gst_element_link_filtered(video_scale_480, video_sink_1, caps_480) != TRUE ||
      gst_element_link (video_queue_360, video_scale_360) != TRUE || gst_element_link_filtered(video_scale_360, video_sink_2, caps_360) != TRUE) {
    g_printerr ("Elements could not be linked.\n");
    gst_object_unref (pipeline);
    return -1;
  }
  gst_caps_unref(caps_720);
  gst_caps_unref(caps_480);
  gst_caps_unref(caps_360);

  /* linking first (audio) tee */
  audio_tee_audio_pad = gst_element_get_request_pad (audio_tee, "src_%u");
  audio_tee_video_pad = gst_element_get_request_pad (audio_tee, "src_%u");
  queue_audio_pad = gst_element_get_static_pad (audio_queue, "sink");
  queue_video_pad = gst_element_get_static_pad (video_queue, "sink");
  if (gst_pad_link (audio_tee_audio_pad, queue_audio_pad) != GST_PAD_LINK_OK ||
      gst_pad_link (audio_tee_video_pad, queue_video_pad) != GST_PAD_LINK_OK) {
    g_printerr ("Audio tee could not be linked.\n");
    gst_object_unref (pipeline);
    return -1;
  }

  /* linking video tee */
  tee_720_video_pad = gst_element_get_request_pad (video_tee, "src_%u");
  g_print ("Obtained request pad %s for video branch.\n", gst_pad_get_name (tee_720_video_pad));
  tee_480_video_pad = gst_element_get_request_pad (video_tee, "src_%u");
  g_print ("Obtained request pad %s for video branch.\n", gst_pad_get_name (tee_480_video_pad));
  tee_360_video_pad = gst_element_get_request_pad (video_tee, "src_%u");
  g_print ("Obtained request pad %s for video branch.\n", gst_pad_get_name (tee_360_video_pad));
  // tee_rtmp_video_pad = gst_element_get_request_pad (video_tee, "src_%u");

  queue_720_video_pad = gst_element_get_static_pad (video_queue_720, "sink");
  queue_480_video_pad = gst_element_get_static_pad (video_queue_480, "sink");
  queue_360_video_pad = gst_element_get_static_pad (video_queue_360, "sink");

  if (gst_pad_link (tee_720_video_pad, queue_720_video_pad) != GST_PAD_LINK_OK ||
      gst_pad_link (tee_480_video_pad, queue_480_video_pad) != GST_PAD_LINK_OK ||
      gst_pad_link (tee_360_video_pad, queue_360_video_pad) != GST_PAD_LINK_OK
      ) {
    g_printerr ("Video tee could not be linked.\n");
    gst_object_unref (pipeline);
    return -1;
  }

  /* release static pads */
  gst_object_unref (queue_audio_pad);
  gst_object_unref (queue_video_pad);
  gst_object_unref (queue_720_video_pad);
  gst_object_unref (queue_480_video_pad);
  gst_object_unref (queue_360_video_pad);

  /* start playing the pipeline */
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  /* wait until error or EOS */
  bus = gst_element_get_bus (pipeline);
  msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  /* release request pads */
  gst_element_release_request_pad (audio_tee, audio_tee_audio_pad);
  gst_element_release_request_pad (audio_tee, audio_tee_video_pad);
  gst_object_unref (audio_tee_audio_pad);
  gst_object_unref (audio_tee_video_pad);

  gst_element_release_request_pad (video_tee, tee_720_video_pad);
  gst_element_release_request_pad (video_tee, tee_480_video_pad);
  gst_element_release_request_pad (video_tee, tee_360_video_pad);
  gst_object_unref (tee_720_video_pad);
  gst_object_unref (tee_480_video_pad);
  gst_object_unref (tee_360_video_pad);

  /* free resources */
  if (msg != NULL)
    gst_message_unref (msg);
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);

  gst_object_unref (pipeline);
  return 0;
}