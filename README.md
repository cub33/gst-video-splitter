# GStreamer Video Splitter

A simple video splitter built with Gstreamer which displays a sound noise into three different resolutions: 1280x720, 848x480, 640x360.

One of the outputs can be configured to stream to an RTMP server.

### Structure:
```
             audio_queue---audio_sink
            /
audiosrc---O                              --video_queue_720---video_sink_0
            \                           /
             video_queue---wavescope---O---video_queue_480---video_sink_1
                                        \
                                         \--video_queue_360---video_sink_2
                                          \
                                           --video_queue_rtmp---rtmp_sink
```
*Some elements are not shown in the graph  above

### Pipeline
```
gst-launch-1.0 audiotestsrc wave=pink-noise ! spacescope ! videoconvert ! tee name=out ! queue ! videoscale ! video/x-raw,width=1280,height=720 ! autovideosink out. ! queue ! videoscale ! video/x-raw,width=720,height=480 ! autovideosink out. ! queue ! videoscale ! video/x-raw,width=480,height=360 ! autovideosink out. ! queue ! videoscale ! video/x-raw,width=1920,height=1980 ! avenc_flv ! flvmux ! rtmpsink location='rtmp://localhost/live/123'
```

## How to play
### Build
```
$ cd build
$ cmake ..
$ cmake --build .
```
### Run
```
$ ./gst-splitter
```

If you want to test the RTMP stream locally on your machine, you can use the Docker nginx RTMP module image as local RTMP server. Run ```docker run -d -p 1935:1935 tiangolo/nginx-rtmp``` to create the local server and then add the RTMP endpoint with a random stream key when running gst-splitter. Example:
`./gst-splitter rtmp://localhost/live/123`.

To make sure that the RTMP stream is working as expected, open VLC, hit `ctrl+n`, and paste the same RTMP URL you added while running the program. 