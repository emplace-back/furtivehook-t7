#include "dependencies/std_include.hpp"
#include "structs.hpp"

namespace game 
{
	void bdCommonAddr::get_local_addr(void* buffer) const
	{
		auto* common_adr = this;
		
		game::call(0x7FF6C672AE00, &common_adr); // dwGetLocalCommonAddr

		if (common_adr)
		{
			game::call(0x7FF6C7BE1560, common_adr, buffer); // bdCommonAddr::serialize
		}
	}
	
	void netadr_t::set_inaddr(const uint32_t inaddr)
	{
		const auto result = ::ntohl(inaddr);

		this->ip[0] = (result >> 24) & 0xFF;
		this->ip[1] = (result >> 16) & 0xFF;
		this->ip[2] = (result >> 8) & 0xFF;
		this->ip[3] = result & 0xFF;
	}
	
	std::string bdAddr::to_string() const
	{
		char buffer[0x40] = { 0 };
		game::call(0x7FF6C7BE0B10, this, buffer, sizeof buffer);

		return std::string{ buffer, sizeof buffer };
	}
	
	netadr_t bdAddrHandle::get_addr() const
	{
		const char* error_msg{};
		const auto inaddr = game::call<uint32_t>(0x7FF6C671BCE0, &this->endpoint, &error_msg);

		if (inaddr)
		{
			game::netadr_t netadr{};
			netadr.type = game::NA_IP;
			netadr.port = 4986;
			netadr.set_inaddr(inaddr);

			return netadr;
		}

		return {};
	}
	
	XNADDR netadr_t::to_xnaddr() const
	{
		game::XNADDR xn;
		game::dwNetadrToCommonAddr(*this, &xn, sizeof xn, nullptr);

		return xn;
	}

	bool netadr_t::connected() const
	{
		return game::dwGetConnectionStatus(this) == game::BD_SOCKET_CONNECTED;
	}

	bool netadr_t::local() const
	{
		return this->type == game::NA_LOOPBACK;
	}
}
