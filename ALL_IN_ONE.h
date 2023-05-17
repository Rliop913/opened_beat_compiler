#pragma once
#include "json/json.h"
#include <string>
#include <vector>
#include <fstream>
#include <functional>
#include <unordered_map>
#define CONST_BPM 60
class ALL_IN_ONE
{


public:
	typedef double start_time;
	typedef double start_bpm;
	typedef std::string song_data_path;
	typedef std::string note_data_path;
	typedef std::string standard_tag_list;
	typedef int graphical_length_between_beat;
	//~ALL_IN_ONE();

private:
	song_data_path song_data = "";
	note_data_path note_data = "";
	
	
	

private:
	Json::Value root;
	Json::Reader parser;
private:
	void song_data_collector(std::ifstream* file_pointer);
	void init_ch_bpm(std::ifstream* file_pointer);
	void sort_bpm_storage();
	void record_between_bpm();
	void note_reading_start(std::ifstream *file_pointer);
	
private:
	struct stored_data
	{
		start_time time = 0.0;
		start_bpm bpm = 0.0;
		graphical_length_between_beat length = 1;
		standard_tag_list std_tag[3] = { "line_number","separate","y"};
		standard_tag_list std_long_tag[6] = {"s_line_number","s_separate","s_y","e_line_number","e_separate","e_y"};
		standard_tag_list std_effect_tag = "ch_bpm";
		void* user;
		
		std::function<void(double,double, std::unordered_map<std::string, std::string>,void* user_data)> normal_callback;//time,graphical location, tag list
		std::function<void(double, double,double,double, std::unordered_map<std::string, std::string>, void* user_data)> long_callback;//start time, end time, start_location, end_location, tag list

	}data_storage;

	struct standard_tag_table
	{
		int line_number;
		int separate;
		int y;
	};

	struct ch_bpm_data_table
	{
		standard_tag_table std_table;
		double time_to_here = 0.0;
		double approx_loc = 0.0;
		double bpm=0;
		int frame_to_here = 0;
	};

	
	

	std::vector<ch_bpm_data_table> bpm_change_storage;
private:
	double calc_time(standard_tag_table std_dat, double bpm);
	double calc_time(double approx_loc, double bpm);
	double calc_time_between(ch_bpm_data_table front_dat, standard_tag_table back_dat);
private:
	double calc_loc(standard_tag_table std_dat);
	double calc_loc(double approx_loc);
	double to_approx(standard_tag_table table);
public:
	void song_data_collector(std::ifstream* file_pointer, double *bpm__, double *start_time__);
	ALL_IN_ONE(
			song_data_path s_json,note_data_path d_json,
			graphical_length_between_beat g_length,
			std::function<void(double,double, std::unordered_map<std::string, std::string>,void*)> normal_callback_,
			std::function<void(double, double, double, double, std::unordered_map<std::string, std::string>,void*)> long_callback_,void* user_data
	);
	ALL_IN_ONE();
};


ALL_IN_ONE::ALL_IN_ONE(song_data_path s_json, note_data_path d_json, graphical_length_between_beat g_length, std::function<void(double, double, std::unordered_map<std::string, std::string>, void*)> normal_callback_, std::function<void(double, double, double, double, std::unordered_map<std::string, std::string>, void*)> long_callback_, void* user_data) {
	song_data = s_json;
	note_data = d_json;
	data_storage.length = g_length;
	if (normal_callback_) {
		data_storage.normal_callback = normal_callback_;
	}
	else {
		data_storage.normal_callback = NULL;
	}
	if (long_callback_) {
		data_storage.long_callback = long_callback_;
	}
	else {
		data_storage.long_callback = NULL;
	}
	if (user_data != nullptr) {
		data_storage.user = user_data;
	}
	if (song_data != "") {
		std::ifstream jfile(song_data);
		song_data_collector(&jfile);
	}
	if (note_data != "") {
		std::ifstream ch_bpm_pointer(note_data);
		std::ifstream note_pointer(note_data);
		init_ch_bpm(&ch_bpm_pointer);
		note_reading_start(&note_pointer);
	}
}

void
ALL_IN_ONE::note_reading_start(std::ifstream* file_pointer) {
	std::string temp;
	while (std::getline(*file_pointer, temp)) {
		if (parser.parse(temp, root)) {
			if (root.isMember(data_storage.std_effect_tag)) {
				continue;
			}
			else if (root.isMember(data_storage.std_long_tag[0])) {//long table
				std::unordered_map<std::string, std::string> dict;
				standard_tag_table start_table;
				standard_tag_table end_table;
				for (auto it = root.begin(); it != root.end(); ++it) {
					if (it.key().asString() == "s_line_number") {
						start_table.line_number = std::stoi((*it).asString());
					}
					else if (it.key().asString() == "e_line_number") {
						end_table.line_number = std::stoi((*it).asString());
					}
					else if (it.key().asString() == "e_separate") {
						end_table.separate = std::stoi((*it).asString());
					}
					else if (it.key().asString() == "e_y") {
						end_table.y = std::stoi((*it).asString());
					}
					else if (it.key().asString() == "s_separate") {
						start_table.separate = std::stoi((*it).asString());
					}
					else if (it.key().asString() == "s_y") {
						start_table.y = std::stoi((*it).asString());
					}
					else {
						dict[it.key().asString()] = (*it).asString();
					}
				}
				if (bpm_change_storage.size() == 0) {//no bpm changes
					data_storage.long_callback(calc_time(start_table, data_storage.bpm), calc_time(end_table, data_storage.bpm), calc_loc(start_table), calc_loc(end_table), dict, data_storage.user);
				}
				else {//bpm changed
					double start_approx = to_approx(start_table);
					double end_approx = to_approx(end_table);
					int start_bpm_point = 0;
					int end_bpm_point = 0;
					if (start_approx > bpm_change_storage[bpm_change_storage.size() - 1].approx_loc) {//no more higher approx
						start_bpm_point = bpm_change_storage.size() - 1;
					}
					else if (start_approx < bpm_change_storage[0].approx_loc) {//lower than every bpm ch
						start_bpm_point = 0;
					}
					else {
						for (int i = 1; i < bpm_change_storage.size(); i++) {
							if (start_approx < bpm_change_storage[i].approx_loc) {
								start_bpm_point = i - 1;
							}
						}
					}
					if (end_approx > bpm_change_storage[bpm_change_storage.size() - 1].approx_loc) {//no more higher approx
						end_bpm_point = bpm_change_storage.size() - 1;
					}
					else if (end_approx < bpm_change_storage[0].approx_loc) {//lower than every bpm ch
						end_bpm_point = 0;
					}
					else {
						for (int i = 1; i < bpm_change_storage.size(); i++) {
							if (end_approx < bpm_change_storage[i].approx_loc) {
								end_bpm_point = i - 1;
							}
						}
					}
					data_storage.long_callback(calc_time_between(bpm_change_storage[start_bpm_point], start_table), calc_time_between(bpm_change_storage[end_bpm_point], end_table), calc_loc(start_table), calc_loc(end_table), dict, data_storage.user);

				}

			}
			else if (root.isMember(data_storage.std_tag[0])) {//normal table
				std::unordered_map<std::string, std::string> dict;

				standard_tag_table temp_table;
				for (auto it = root.begin(); it != root.end(); ++it) {
					if (it.key().asString() == "line_number") {
						temp_table.line_number = std::stoi((*it).asString());
					}
					else if (it.key().asString() == "separate") {
						temp_table.separate = std::stoi((*it).asString());
					}
					else if (it.key().asString() == "y") {
						temp_table.y = std::stoi((*it).asString());
					}
					else {
						dict[it.key().asString()] = (*it).asString();
					}
				}
				if (bpm_change_storage.size() == 0) {//no bpm changes
					data_storage.normal_callback(calc_time(temp_table, data_storage.bpm), calc_loc(temp_table), dict, data_storage.user);
				}
				else {//bpm changed
					double temp_approx = to_approx(temp_table);
					if (temp_approx > bpm_change_storage[bpm_change_storage.size() - 1].approx_loc) {//no more higher approx
						data_storage.normal_callback(calc_time_between(bpm_change_storage[bpm_change_storage.size() - 1], temp_table), calc_loc(temp_approx), dict, data_storage.user);
					}
					else if (temp_approx < bpm_change_storage[0].approx_loc) {//lower than every bpm ch
						data_storage.normal_callback(calc_time(temp_approx, data_storage.bpm), calc_loc(temp_approx), dict, data_storage.user);
					}
					else {
						for (int i = 1; i < bpm_change_storage.size(); i++) {
							if (temp_approx < bpm_change_storage[i].approx_loc) {
								data_storage.normal_callback(calc_time_between(bpm_change_storage[i - 1], temp_table), calc_loc(temp_approx), dict, data_storage.user);
							}
						}
					}
				}
			}
			else {
				//no available data error
			}
		}
	}
}


void
ALL_IN_ONE::init_ch_bpm(std::ifstream* file_pointer) {
	std::string temp;
	while (std::getline(*file_pointer, temp)) {
		if (parser.parse(temp, root)) {
			if (root.isMember(data_storage.std_effect_tag)) {
				ch_bpm_data_table temp_table;
				temp_table.std_table.line_number = std::stoi(root["line_number"].asString());
				temp_table.std_table.separate = std::stoi(root["separate"].asString());
				temp_table.std_table.y = std::stoi(root["y"].asString());
				temp_table.bpm = std::stod(root[data_storage.std_effect_tag].asString());
				bpm_change_storage.push_back(temp_table);
			}
			else {
				continue;
			}
		}
		root.clear();
	}
	sort_bpm_storage();
	record_between_bpm();
}

double
ALL_IN_ONE::to_approx(standard_tag_table table) {
	return double(table.line_number) + (double(table.y) / double(table.separate));
}

void
ALL_IN_ONE::record_between_bpm() {
	double accumulator = 0.0;
	if (bpm_change_storage.size() != 0) {
		bpm_change_storage[0].time_to_here = calc_time(bpm_change_storage[0].approx_loc, data_storage.bpm);
	}
	for (int i = 1; i < bpm_change_storage.size(); i++) {
		bpm_change_storage[i].time_to_here = calc_time_between(bpm_change_storage[i - 1], bpm_change_storage[i].std_table);
	}
}


double
ALL_IN_ONE::calc_time(double approx_loc, double bpm) {
	double final_value = data_storage.time;
	final_value += approx_loc * (double(CONST_BPM) / bpm);
	return final_value;
}


double
ALL_IN_ONE::calc_time(standard_tag_table std_dat, double bpm) {
	double final_value = data_storage.time;
	double approx_loc = to_approx(std_dat);
	final_value += approx_loc * (double(CONST_BPM) / bpm);
	return final_value;
}

double
ALL_IN_ONE::calc_time_between(ch_bpm_data_table front_dat, standard_tag_table back_dat) {
	double between_time = 0.0;
	double temp_approx = to_approx(back_dat) - to_approx(front_dat.std_table);
	between_time = calc_time(temp_approx, front_dat.bpm) + front_dat.time_to_here;
	return between_time;
}
void
ALL_IN_ONE::sort_bpm_storage() {
	for (int i = 0; i < bpm_change_storage.size(); i++) {
		bpm_change_storage[i].approx_loc = to_approx(bpm_change_storage[i].std_table);
	}
	double key = 0;
	int j = 0;
	for (int i = 1; i < bpm_change_storage.size(); i++) {
		key = bpm_change_storage[i].approx_loc;
		for (j = i - 1; j >= 0 && bpm_change_storage[j].approx_loc > key; j--) {
			bpm_change_storage[j + 1].approx_loc = bpm_change_storage[j].approx_loc;
		}
		bpm_change_storage[j + 1].approx_loc = key;
	}
}

void
ALL_IN_ONE::song_data_collector(std::ifstream* file_pointer) {
	*file_pointer >> root;
	data_storage.bpm = std::stod(root["bpm"].asString());
	data_storage.time = std::stod(root["start_time"].asString());
}

double
ALL_IN_ONE::calc_loc(standard_tag_table std_dat) {
	double approx_loc = to_approx(std_dat);
	return approx_loc * data_storage.length;
}

double
ALL_IN_ONE::calc_loc(double approx_loc) {
	return approx_loc * data_storage.length;
}

void
ALL_IN_ONE::song_data_collector(std::ifstream* file_pointer, double* bpm__, double* start_time__) {
	*file_pointer >> root;
	*bpm__ = std::stod(root["bpm"].asString());
	*start_time__ = std::stod(root["start_time"].asString());
}

ALL_IN_ONE::ALL_IN_ONE() {

}