# 파일 경로 지정
input_file_path = "setting_page.html"  # 입력 파일 경로
output_file_path = "output_hex.txt"    # 출력 파일 경로

# HTML 파일을 바이너리 모드로 읽기
with open(input_file_path, 'rb') as file:
    html_bytes = file.read()  # 원본 데이터 (바이너리)

# Hex 값으로 변환
hex_values = [f"0x{byte:02X}" for byte in html_bytes]

# 결과를 txt 파일로 저장
with open(output_file_path, 'w', encoding='latin-1') as output_file:
    output_file.write(f"static const uint8_t _acindex[{len(hex_values)}] = {{\n")
    for i, hex_value in enumerate(hex_values):
        output_file.write(f"  {hex_value},")
        if (i + 1) % 12 == 0:  # 12개마다 줄바꿈
            output_file.write("\n")
    output_file.write("\n};\n")

print(f"✅ 변환된 결과가 '{output_file_path}' 파일에 저장되었습니다.")

# 변환된 데이터 검증
with open(output_file_path, 'r', encoding='latin-1') as file:
    converted_data = file.read()

# 변환된 Hex 데이터를 다시 바이너리로 변환
converted_bytes = bytes(int(x, 16) for x in converted_data.split(",") if x.strip().startswith("0x"))
