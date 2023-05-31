# opened_beat_compiler
beat compiler for musical programs like rhythm games


if you want to use this program, check the example.json(about song_data format) and example2.json(about note_data format)

how to use

1.import to your project

2.make three functions for normal_datacallback, long_datacallback, bpm_datacallback
  
	2-1.void normal_DCB(
	time:double,
	graphical_location:double,
	user_tags:std::unordered_map<std::string, std::string>,
	user_pointer:void*
	)
  
  
	2-2.void long_DCB(
	start_time:double,
	end_time:double,
	start_graphical_loc:double,
	end_graphical_loc,
	user_tags:std::unordered_map<std::string, std::string>,
	user_pointer:void*
	)
	
	2-3.void bpm_DCB(
	time:double, 
	graphical_location:double, 
	user_tags:std::unordered_map<std::string, std::string>,
	change_bpm:double,
	user_pointer:void*
	)
	
3.fill init_group
	struct init_group {
		song_data_path s_json;
		note_data_path d_json;
		graphical_length_between_beat g_length;
		std::function<void(double, double, std::unordered_map<std::string, std::string>, void*)> normal_callback_;//time, graphical location, tag list, user's pointer
		std::function<void(double, double, double, double, std::unordered_map<std::string, std::string>, void*)> long_callback_;//start time, end time, start graphic, end graphic, tag list, user's pointer
		std::function<void(double, double, std::unordered_map<std::string, std::string>, double, void*)> bpm_callback_;//time,graphical location, tag list, changed bpm,user's pointer
		void* user_data;
	};
	

3.add line"ALL_IN_ONE class_name = ALL_IN_ONE(init_group your_group)"
  
  
	3-1.graphical location is for rhythm game developer, it means the height at which the note is located graphically.
  
  
	3-2. the graphical_length means the graphic length between note to note. fix to 1 if you want to ignore
  
  
	3-3.only one callback function will be called every reading one note
