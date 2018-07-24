// The original post on stackoverflow is as follows:

/*
It is a matter of calculating backwards. From the front vector you should be able to deduce yaw and pitch relativly easy.

If your coordinate system has x and z as the floor

yaw = atan2(front.z, front.x);

The pitch can be calculated by normalizing front and using asin

front.normalize();
pitch = asin(front.y);

If I have time later I'll look into calculating the roll

Otherwise has shoosh a good suggestion to use gluLookAt

				epatel
*/

yaw = atan2(front.z, front.x);
front.normalize();
pitch = asin(front.y);
