# course-requirement-checker

## How to use it?
  1. Modify `./course_list_sample` to your own course_list. 
  2. Type `g++ check.cpp -o checker` to compile it.
  3. Run `./checker < course_list` to check your course list.
  
## How to generate my own course_list
  1. `cp course_list_sample course_list`.
  2. Modify the first line of `./course_list` to your own track.
  3. Add your own course in `./course_list` by the style of `./course_list_sample`, the course number is enough.
  4. If one course is wavied, you just need to add a minus sign before the course number, check the `./course_list_sample`. 
