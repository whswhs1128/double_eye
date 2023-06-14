typedef struct {
  rtsp_demo_handle g_rtsplive;
  rtsp_session_handle session;
  int channel_num;
}rtsp_handle_struct;

struct rtsp_handle_struct get_struct();
