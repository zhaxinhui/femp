Point(1) = {-2, -1, -1, 1e+22};
Point(2) = {2, -1, -1, 1e+22};
Point(3) = {-2, 1, -1, 1e+22};
Point(4) = {2, 1, -1, 1e+22};
Point(5) = {2, 1, 0, 1e+22};
Point(6) = {2, -1, 0, 1e+22};
Point(10) = {-2, -1, 0, 1e+22};
Point(14) = {-2, 1, 0, 1e+22};
Line(1) = {2, 1};
Line(2) = {1, 3};
Line(3) = {3, 4};
Line(4) = {4, 2};
Line(8) = {5, 6};
Line(9) = {6, 10};
Line(10) = {10, 14};
Line(11) = {14, 5};
Line(13) = {4, 5};
Line(14) = {2, 6};
Line(18) = {1, 10};
Line(22) = {3, 14};
Line Loop(6) = {4, 1, 2, 3};
Plane Surface(6) = {6};
Line Loop(15) = {4, 14, -8, -13};
Ruled Surface(15) = {15};
Line Loop(19) = {1, 18, -9, -14};
Ruled Surface(19) = {19};
Line Loop(23) = {2, 22, -10, -18};
Ruled Surface(23) = {23};
Line Loop(27) = {3, 13, -11, -22};
Ruled Surface(27) = {27};
Line Loop(28) = {8, 9, 10, 11};
Plane Surface(28) = {28};
Surface Loop(1) = {6, 28, 15, 19, 23, 27};
Volume(1) = {1};