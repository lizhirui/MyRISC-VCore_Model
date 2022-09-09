from itertools import count
from logging import exception
import re

f_in = open("trace_default_gen_in.txt", "r")
lines = f_in.readlines()
f_in.close()
f_out = open("trace_default_gen_out_tb.txt", "w")

cur_count_text = ""
tab_unit = " " * 4
cur_tab = ""

space_num_max = 0

cnt = 0
for line in lines:
    cnt = cnt + 1
    space_num_max = max(space_num_max, len(line) - len(line.lstrip(' ')))

cur_tab = " " * space_num_max
split_tab = cur_tab

out_buf = ""

for line in lines:
    try:
        line = line.replace("EXECUTE_UNIT_NUM", "WB_WIDTH")

        if len(line.strip()) == 0:
            line = cur_tab[len(split_tab):] + line
        else:
            count_text = ""
            
            if not "1)" in line:
                count_text = re.search(r"([^,\n]+)\);", line).group(1).strip()

            if count_text != cur_count_text:
                if cur_count_text != "":
                    cur_tab = cur_tab[len(tab_unit):]
                    out_buf += cur_tab + "end\n\n"

                if count_text != "":
                    if cur_count_text == "":
                        out_buf += cur_tab + "\n"

                    if not count_text.isdigit():
                        out_buf += cur_tab + "for(i = 0;i < `" + count_text + ";i++) begin\n"
                    else:
                        out_buf += cur_tab + "for(i = 0;i < " + count_text + ";i++) begin\n"
                    
                    cur_tab += tab_unit

                cur_count_text = count_text

            line = cur_tab[len(split_tab):] + line
            
            if "domain_t::input" in line:
                domain = "DOMAIN_INPUT"
            elif "domain_t::output" in line:
                domain = "DOMAIN_OUTPUT"
            elif "domain_t::status" in line:
                domain = "DOMAIN_STATUS"
            else:
                raise exception("Unknown domain " + line)
            
            name = re.search(r"\"([^\"]+)\"", line).group(1)
            function_name = re.search(r"([A-Za-z_0-9]+)\(", line).group(1)
            arg_list_str = re.search(r"\(([^;]+)\)", line).group(1)
            arg_list = arg_list_str.split(",")
            arg_list = [x.strip() for x in arg_list]
            size = arg_list[len(arg_list) - 2]
            num = arg_list[len(arg_list) - 1]

            if size == "sizeof(uint8_t)":
                size = "8"
            elif size == "sizeof(uint16_t)":
                size = "16"
            elif size == "sizeof(uint32_t)":
                size = "32"
            elif size == "sizeof(uint64_t)":
                size = "64"

            if not size.isdigit():
                size = "`" + size

            if not num == "1":
                num = "i"

                if "." in name:
                    var_name = name.replace(".", "[i].")
                else:
                    var_name = name + "[i]"
            else:
                num = "0"
                var_name = name

            if size.isdigit():
                func_str = "tdb.get_uint" + size + "(" + domain + ", \"" + name + "\", " + num + ")"
            else:
                func_str = "tdb_reader::get_vector#(" + size + ")::_do(tdb, " + domain + ", \"" + name + "\", " + num + ")"

            if domain == "DOMAIN_INPUT":
                line = cur_tab + var_name + " = " + func_str + ";"
            else:
                line = cur_tab + "`assert_equal(cur_cycle, " + func_str + ", " + var_name + ");"
    except Exception as e:
        raise e
    
    if line[-1] != "\n":
        line += "\n"

    out_buf += line

if cur_count_text != "":
    cur_tab = cur_tab[len(tab_unit):]
    out_buf += cur_tab + "end\n\n"

old_out_buf = out_buf

while True:
    replace_group = re.findall(r"[\n]+[ ]*[\n]+[ ]*\}", out_buf)
    
    for item in replace_group:
        replace_text = "\n" + re.search(r"[ ]*\}", item).group()
        out_buf = out_buf.replace(item, replace_text)

    if out_buf == old_out_buf:
        break

    old_out_buf = out_buf

f_out.write(out_buf)
f_out.close()
