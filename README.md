# opened_beat_compiler
beat compiler for musical programs like rhythm games; 


if you want to use this program, you need to follow beat_format
check the example.json(about song_data format) and example2.json(about note_data format)

how to use

1.import to your project

2.make two functions for normal_datacallback, long_datacallback
  2-1.void normal_DCB(time:double, graphical_location:double, user_tags:std::vector<ALL_IN_ONE::custom_tag_list>)
  2-2.void long_DCB(start_time:double,end_time:double,start_graphical_loc:double,end_graphical_loc,  user_tags:std::vector<ALL_IN_ONE::custom_tag_list>)

3.add line"ALL_IN_ONE class_name = ALL_IN_ONE(song_data_path, note_data_path, graphical_length:int, normal_datacallback, long_datacallback)"
  3-1.graphical location is for rhythm game developer, it means the height at which the note is located graphically.
  3-2. the graphical_length means the graphic length between note to note that fix to 1 if you want to ignore
  3-3.only one callback function will be called every reading one note
