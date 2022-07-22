import re

f_in = open("trace_default_gen_in.txt", "r")
lines = f_in.readlines()
f_in.close()
f_out = open("trace_default_gen_out.txt", "w")

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
        if len(line.strip()) == 0:
            line = cur_tab[len(split_tab):] + line
        else:
            count_text = ""
            
            if not "1)" in line:
                count_text = re.search(r"([^,\n]+)\);", line).group(1).strip()

            if count_text != cur_count_text:
                if cur_count_text != "":
                    cur_tab = cur_tab[len(tab_unit):]
                    out_buf += cur_tab + "}\n\n"

                if count_text != "":
                    if cur_count_text == "":
                        out_buf += cur_tab + "\n"
                        
                    out_buf += cur_tab + "for(auto i = 0;i < " + count_text + ";i++)\n"
                    out_buf += cur_tab + "{\n"
                    cur_tab += tab_unit

                cur_count_text = count_text

            line = cur_tab[len(split_tab):] + line

            if "mark_signal_bitmap" in line:
                line = line.replace("mark_signal_bitmap", "update_signal_bitmap_all")
                ori_str = re.search(r"[0-9A-Za-z_]+, [^,\n]+\);", line).group()
            else:
                type_str = re.search(r"sizeof\(([0-9A-Za-z_]+)\)", line).group(1)
                line = line.replace("mark_signal", "update_signal<" + type_str + ">")
                ori_str = re.search(r"sizeof\([0-9A-Za-z_]+\), [^,\n]+\);", line).group()

            if count_text == "":
                line = line.replace(ori_str, "0, 0);")
            else:
                line = line.replace(ori_str, "0, i);")
    except:
        pass
    
    if line[-1] != "\n":
        line += "\n"

    out_buf += line

if cur_count_text != "":
    cur_tab = cur_tab[len(tab_unit):]
    out_buf += cur_tab + "}\n\n"

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
