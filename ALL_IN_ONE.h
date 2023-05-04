#pragma once
#include "json/json.h"
#include <string>
#include <vector>
#include <fstream>
#include <functional>
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

	struct custom_tag_list 
	{
		std::string key;
		std::string value;
	};
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

		
		std::function<void(double,double, std::vector<custom_tag_list>)> normal_callback;//time,graphical location, tag list
		std::function<void(double, double,double,double, std::vector<custom_tag_list>)> long_callback;//start time, end time, start_location, end_location, tag list

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
	};

	
	

	std::vector<ch_bpm_data_table> bpm_change_storage;
private:
	double calc_time(standard_tag_table std_dat, double bpm);
	double calc_time(double approx_loc, double bpm);
	double calc_loc(standard_tag_table std_dat);
	double calc_loc(double approx_loc);
	double calc_time_between(ch_bpm_data_table front_dat, standard_tag_table back_dat);
	double to_approx(standard_tag_table table);
public:

	ALL_IN_ONE(song_data_path s_json,note_data_path d_json, graphical_length_between_beat g_length, std::function<void(double,double, std::vector<custom_tag_list>)> normal_callback_, std::function<void(double, double, double, double, std::vector<custom_tag_list>)> long_callback_);

};

