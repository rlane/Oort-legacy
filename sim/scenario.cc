// Copyright 2011 Rich Lane
#include "sim/scenario.h"

#include <stdexcept>
#include <boost/foreach.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include "json_spirit_reader_template.h"
#include "json_spirit_reader.h"
#include "common/resources.h"

namespace Oort {

using namespace glm;

Scenario::Scenario() {
}

Scenario Scenario::load(std::string path) {
	Scenario scn;

	if (path == "test/missile.json") {
		{
			scn.teams.emplace_back();
			auto &team = scn.teams.back();
			team.name = "green";
			team.color = vec3(0, 1, 0);
			{
				team.ships.emplace_back();
				auto &s = team.ships.back();
				s.klass = "fighter";
				s.p = vec2(0, 0);
				s.v = vec2(0, 0);
				s.h = 0;
			}
		}

		{
			scn.teams.emplace_back();
			auto &team = scn.teams.back();
			team.name = "red";
			team.color = vec3(1, 0, 0);
			{
				team.ships.emplace_back();
				auto &s = team.ships.back();
				s.klass = "ion_cannon_frigate";
				s.p = vec2(500, 0);
				s.v = vec2(0, 0);
				s.h = 1.57;
			}
		}
	} else if (path == "test/move.json") {
		{
			scn.teams.emplace_back();
			auto &team = scn.teams.back();
			team.name = "green";
			team.color = vec3(0, 1, 0);
			{
				team.ships.emplace_back();
				auto &s = team.ships.back();
				s.klass = "target";
				s.p = vec2(0, 0);
				s.v = vec2(0, 0);
				s.h = 0;
			}
		}
	} else {
		load_json(scn, path);
	}

	return scn;
}

void Scenario::load_json(Scenario &scn, std::string path) {
	auto data = load_resource(path);
	json_spirit::mValue value;
	json_spirit::read_string(data, value);
	json_spirit::mObject &jscn = value.get_obj();

	scn.description = jscn.find("description")->second.get_str();
	scn.author = jscn.find("author")->second.get_str();

	json_spirit::mArray teams = jscn.find("teams")->second.get_array();
	BOOST_FOREACH(json_spirit::mValue &e, teams) {
		auto jteam = e.get_obj();
		scn.teams.emplace_back();
		auto &team = scn.teams.back();

		team.name = jteam.find("name")->second.get_str();

		auto jcolor = jteam.find("color")->second.get_obj();
		team.color.r = jcolor.find("red")->second.get_real();
		team.color.g = jcolor.find("green")->second.get_real();
		team.color.b = jcolor.find("blue")->second.get_real();

		auto jships = jteam.find("ships")->second.get_array();
		BOOST_FOREACH(json_spirit::mValue &e, jships) {
			auto jship = e.get_obj();
			team.ships.emplace_back();
			auto &ship = team.ships.back();

			ship.klass = jship.find("klass")->second.get_str();

			auto jpos = jship.find("p")->second.get_array();
			ship.p.x = jpos[0].get_real();
			ship.p.y = jpos[1].get_real();

			auto jvel = jship.find("v")->second.get_array();
			ship.v.x = jvel[0].get_real();
			ship.v.y = jvel[1].get_real();

			ship.h = jship.find("h")->second.get_real();
		}
	}
}

}
