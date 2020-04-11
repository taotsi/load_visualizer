#include <iostream>
#include <memory>
#include "hostgui.h"
#include "transceiver.h"

int main(int, char**){
	HostGui host;
	std::shared_ptr<HostGui> host_ptr;
	auto cpn_ptr = std::make_shared<Component>();
	Transceiver tc;
	tc.AddSubscriber({MsgType::kOut, "msgtest"}, cpn_ptr);
	while(tc.is_on());

	std::cout << "~done~\n";
	return 0;
}
