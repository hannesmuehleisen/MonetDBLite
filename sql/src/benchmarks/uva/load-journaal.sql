
COPY 50000 RECORDS INTO VxSegmentation_tab from 'PWD/journaal/VxSegmentation_tab.txt' USING DELIMITERS ',','\n';
COPY 50000 RECORDS INTO VxSegment_tab from 'PWD/journaal/VxSegment_tab.txt' USING DELIMITERS ',','\n';
COPY 50000 RECORDS INTO VxSegmentStringFeature_tab from 'PWD/journaal/VxSegmentStringFeature_tab.txt' USING DELIMITERS ',','\n';
COPY 50000 RECORDS INTO VxSegmentDoubleFeature_tab from 'PWD/journaal/VxSegmentDoubleFeature_tab.txt' USING DELIMITERS ',','\n';
COPY 50000 RECORDS INTO VxSegmentIntFeature_tab from 'PWD/journaal/VxSegmentIntFeature_tab.txt' USING DELIMITERS ',','\n';
