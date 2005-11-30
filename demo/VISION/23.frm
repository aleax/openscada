<frame name = "frame1">
 <property name = "FrameName">
  <value>frame1</value>
 </property>
 <property name = "updateCycle">
  <value>1000</value>
 </property>
 <property name = "width">
  <value>100</value>
 </property>
 <property name = "height">
  <value>100</value>
 </property>
 <property name = "rotate">
  <value>0.00</value>
 </property>
 <property name = "scaleX">
  <value>1.00</value>
 </property>
 <property name = "scaleY">
  <value>1.00</value>
 </property>
 <property name = "BackgroundColor">
  <value>4294923647</value>
 </property>
 <items>
  <item class = "10001">
   <property name = "z">
    <value>0</value>
   </property>
   <property name = "ItemName">
    <value>Rectangle</value>
   </property>
   <property name = "x">
    <value>23</value>
   </property>
   <property name = "y">
    <value>54</value>
   </property>
   <property name = "width">
    <value>60</value>
    <dynamic>proportion{::0,0;::60,0;::'TimeSYS'.'sec';}</dynamic>
   </property>
   <property name = "height">
    <value>11</value>
   </property>
   <property name = "visible">
    <value>true</value>
   </property>
   <property name = "angle">
    <value>0</value>
   </property>
   <property name = "LineColor">
    <value>4278190080</value>
   </property>
   <property name = "BackgroundColor">
    <value>4294923520</value>
   </property>
   <property name = "LineStyle">
    <value>NoPen</value>
   </property>
   <property name = "BrushStyle">
    <value>SolidPattern</value>
   </property>
   <property name = "LineWeight">
    <value>1</value>
   </property>
   <property name = "LineCap">
    <value>FlatCap</value>
   </property>
   <property name = "LineJoin">
    <value>MiterJoin</value>
   </property>
  </item>
  <item class = "10003">
   <property name = "z">
    <value>1</value>
   </property>
   <property name = "ItemName">
    <value>Text</value>
   </property>
   <property name = "x">
    <value>28</value>
   </property>
   <property name = "y">
    <value>37</value>
   </property>
   <property name = "width">
    <value>18</value>
   </property>
   <property name = "height">
    <value>14</value>
   </property>
   <property name = "visible">
    <value>true</value>
   </property>
   <property name = "angle">
    <value>0</value>
   </property>
   <property name = "TextColor">
    <value>4279176975</value>
   </property>
   <property name = "Text">
    <value>text</value>
    <dynamic>value{'test'.'val'::e2;}</dynamic>
   </property>
   <property name = "family">
    <value>XLinSans</value>
   </property>
   <property name = "PointSize">
    <value>10</value>
   </property>
   <property name = "Bold">
    <value>false</value>
   </property>
   <property name = "Italic">
    <value>false</value>
   </property>
   <property name = "Underline">
    <value>false</value>
   </property>
   <property name = "Strikeout">
    <value>false</value>
   </property>
  </item>
  <item class = "10003">
   <property name = "z">
    <value>2</value>
   </property>
   <property name = "ItemName">
    <value>Text</value>
   </property>
   <property name = "x">
    <value>29</value>
   </property>
   <property name = "y">
    <value>7</value>
   </property>
   <property name = "width">
    <value>18</value>
   </property>
   <property name = "height">
    <value>14</value>
   </property>
   <property name = "visible">
    <value>true</value>
   </property>
   <property name = "angle">
    <value>0</value>
   </property>
   <property name = "TextColor">
    <value>4279176975</value>
   </property>
   <property name = "Text">
    <value>text</value>
    <dynamic>enum{"sec != 0"::'TimeSYS'.'sec';"min != 0"::'TimeSYS'.'min';::"defaultText";}</dynamic>
   </property>
   <property name = "family">
    <value>XLinSans</value>
   </property>
   <property name = "PointSize">
    <value>10</value>
   </property>
   <property name = "Bold">
    <value>false</value>
   </property>
   <property name = "Italic">
    <value>false</value>
   </property>
   <property name = "Underline">
    <value>false</value>
   </property>
   <property name = "Strikeout">
    <value>false</value>
   </property>
  </item>
  <item class = "10004">
   <property name = "z">
    <value>3</value>
   </property>
   <property name = "ItemName">
    <value>Text</value>
   </property>
   <property name = "x">
    <value>25</value>
   </property>
   <property name = "y">
    <value>60</value>
   </property>
   <property name = "width">
    <value>54</value>
   </property>
   <property name = "height">
    <value>34</value>
   </property>
   <property name = "visible">
    <value>true</value>
   </property>
   <property name = "angle">
    <value>0</value>
   </property>
   <property name = "fileName">
    <value>qt.xpm</value>
   </property>
  </item>
 </items>
</frame>
