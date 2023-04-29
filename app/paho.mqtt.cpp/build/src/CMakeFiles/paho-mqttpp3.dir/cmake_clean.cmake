file(REMOVE_RECURSE
  "libpaho-mqttpp3.pdb"
  "libpaho-mqttpp3.so"
  "libpaho-mqttpp3.so.1"
  "libpaho-mqttpp3.so.1.2.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/paho-mqttpp3.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
