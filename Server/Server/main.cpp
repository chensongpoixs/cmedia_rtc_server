
#include "cmedia_server.h"
#include <signal.h>
//------------------------------------------------------------------------------
void Stop(int i)
{
	chen::g_media_server.stop();
}

void RegisterSignal()
{
	signal(SIGINT, Stop);
	signal(SIGTERM, Stop);

}

int main(int argc, char* argv[])
{

	RegisterSignal();

	const char* config_filename = "server.cfg";
	if (argc > 1)
	{
		config_filename = argv[1];
	}
	const char* log_path = "./log";
	if (argc > 2)
	{
		log_path = argv[2];
	}
	bool init = chen::g_media_server.init(log_path, config_filename);

	if (init)
	{
		init = chen::g_media_server.Loop();
	}
	else
	{
		return -1;
	}
	chen::g_media_server.destroy();
	if (!init)
	{
		return 1;
	}
	return 0;

	 
}
