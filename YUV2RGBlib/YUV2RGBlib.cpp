
void yv12_to_rgb32(unsigned char * rgb32, int stride, unsigned char * yuv, int width, int height) 
{										//m_width*4
	unsigned long* rgba = (unsigned long*)(rgb32 + (height - 1) * stride);
	stride >>= 2;
	unsigned char* y = yuv;
	unsigned char* u = y + width * height;
	unsigned char* v = u + width * height / 4;
	for ( int i = 0; i < height; i++ )
	{
		for ( int j = 0; j < width; j++ )
		{
			/*
			R= 1.0Y + 0 +1.402(V-128)  
			G= 1.0Y - 0.34413 (U-128)-0.71414(V-128)  
			B= 1.0Y + 1.772 (U-128)+0
			*/
			int cy = y[j];
			int cu = u[j / 2] - 128;
			int cv = v[j / 2] - 128;
			int cr = (int)(cy + 1.402 * cv);
			int cg = (int)(cy - 0.34413 * cu - 0.71414 * cv);
			int cb = (int)(cy + 1.772 * cu);
			unsigned char r = (cr < 0) ? 0 : ((cr > 255) ? 255 : cr);
			unsigned char g = (cg < 0) ? 0 : ((cg > 255) ? 255 : cg);
			unsigned char b = (cb < 0) ? 0 : ((cb > 255) ? 255 : cb);
			rgba[j] = (r << 16) | (g << 8) | (b);
		}
		rgba -= stride;
		y += width;
		if ( (i & 0x1) != 0 )
		{
			u += width / 2;
			v += width / 2;
		}
	}
}

void yv12_diff_to_rgb32(unsigned char * rgb32, int stride, unsigned char * yuv1, unsigned char * yuv2, int width, int height) 
{
	unsigned long* rgba = (unsigned long*)(rgb32 + (height - 1) * stride);
	stride >>= 2;
	unsigned char* y1 = yuv1;
	unsigned char* u1 = y1 + width * height;
	unsigned char* v1 = u1 + width * height / 4;
	unsigned char* y2 = yuv2;
	unsigned char* u2 = y2 + width * height;
	unsigned char* v2 = u2 + width * height / 4;
	for ( int i = 0; i < height; i++ )
	{
		for ( int j = 0; j < width; j++ )
		{
			int cy1 = y1[j];
			int cu1 = u1[j / 2];
			int cv1 = v1[j / 2];
			int cy2 = y2[j];
			int cu2 = u2[j / 2];
			int cv2 = v2[j / 2];
			unsigned char r;
			unsigned char g;
			unsigned char b;
		//	if(cy1 == cy2 )
			if(cy1 == cy2 && cu1 == cu2 && cv1 ==cv2)
		//	if(y1[j] == y2[j] && u1[j/2] == u2[j / 2] && v1[j / 2] == v2[j / 2])
				r=g=b=0;//ºÚÉ«
			else
				r=g=b=255;//°×É«
			rgba[j] = (r << 16) | (g << 8) | (b);
		}
		rgba -= stride;
		y1 += width;
		y2 += width;
		if ( (i & 0x1) != 0 )
	//	if(i%2==1)
		{
			u1 += width / 2;
			v1 += width / 2;
			u2 += width / 2;
			v2 += width / 2;
		}
	}
}