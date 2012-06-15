// ****************************************************************************
// Filename:  GUIFactory.m
// Author:    Florian Hecht
// Date:      2008
// ****************************************************************************

#import <Cocoa/Cocoa.h>

BOOL CocoaFileDialog(BOOL save_dialog, const char** filter, int num_filter, const char* caption, char* filename, int max_length) {
	NSAutoreleasePool* myPool = [[NSAutoreleasePool alloc] init];
	
	NSSavePanel* dlg;
	if (save_dialog) {
		dlg = [NSSavePanel savePanel];
		[dlg setAllowsOtherFileTypes: FALSE];
		[dlg setExtensionHidden: FALSE];
	}
	else
	{
		NSOpenPanel* openDlg = [NSOpenPanel openPanel];
		[openDlg setCanChooseFiles: YES];
		[openDlg setCanChooseDirectories: NO];
		[openDlg setAllowsMultipleSelection: NO];
		
		dlg = openDlg;
	}
	
	NSString* title = [[NSString alloc] initWithCString: caption encoding: NSASCIIStringEncoding];
	[dlg setTitle: title];
	
	NSArray* array = nil;
	if (filter != NULL && num_filter > 0) {
		id* str_array = malloc(num_filter * sizeof(id));
		int i;
		for (i = 0; i < num_filter; i++) {
			str_array[i] = [[NSString alloc] initWithCString: filter[i] encoding: NSASCIIStringEncoding];
		}
		
		array = [NSArray arrayWithObjects: str_array count: num_filter];
		free(str_array);
		
		[dlg setAllowedFileTypes: array];
		
	}
	
	BOOL has_selected_file = FALSE;
	if (save_dialog) {
		if ([dlg runModal] == NSOKButton) { 
			NSString* fileName = [dlg filename];
			[fileName getCString: filename maxLength: max_length encoding: NSASCIIStringEncoding];
			
			has_selected_file = TRUE;
		}
	} else {
		NSOpenPanel* openDlg = (NSOpenPanel*)dlg;
		
		if ([openDlg runModalForTypes: array] == NSOKButton) { 
			NSString* fileName = [dlg filename];
			[fileName getCString: filename maxLength: max_length encoding: NSASCIIStringEncoding];
			
			has_selected_file = TRUE;
		}
	}
	
	[myPool release];
	
	return has_selected_file;
}

BOOL CocoaLoadImage(const char* filename, unsigned char** ptr, int* width, int* height, int* type) {
	NSAutoreleasePool* myPool = [[NSAutoreleasePool alloc] init];
	
	NSString* str = [[NSString alloc] initWithCString: filename encoding: NSASCIIStringEncoding];
	NSData* data = [[NSData alloc] initWithContentsOfFile: str];
	if ([data length] <= 0)
	{
		printf("error: couldn't load file '%s'\n", filename);
		
		[myPool release];
		
		return FALSE;
	}

	NSBitmapImageRep *tempImage = [[NSBitmapImageRep alloc] initWithData: data];
	if (tempImage == nil) {
		printf("error: unsupported image type in file '%s'\n", filename);
		
		[myPool release];
		
		return FALSE;
	}
	
	int w = [tempImage pixelsWide];
	int h = [tempImage pixelsHigh];
	int bitsPerPixel = [tempImage bitsPerPixel];
	int samplesPerPixel = [tempImage samplesPerPixel];
	int bytesPerRow = [tempImage bytesPerRow];
	
	if (!(bitsPerPixel == 8 && samplesPerPixel == 1) && !(bitsPerPixel == 24 && samplesPerPixel == 3) && !(bitsPerPixel == 32)) {
		printf("error: unsupported bitsPerPixel(%d) or samplesPerPixel(%d) in file '%s'\n", bitsPerPixel, samplesPerPixel, filename);
		
		[myPool release];
		
		return FALSE;
	}
	
	*width = w;
	*height = h;
	
	if (samplesPerPixel == 1)
		*type = 1;
	else
		*type = 2;
	
	*ptr = (unsigned char*)malloc(w * h * (*type == 1 ? 1 : 3));
	
	unsigned char *bytes = [tempImage bitmapData];
	if (bytes) {
	
		if (bitsPerPixel != 32) {
		
			if (bytesPerRow == w * samplesPerPixel) {
			
				// simply copy the whole buffer
				memcpy(*ptr, bytes, w * h * samplesPerPixel);
				
			} else {
			
				// we have to copy every single row independently,
				// because the bytesPerRow are different from the actual pixel data per row
				int y;
				unsigned char* row = bytes;
				unsigned char* row_ptr = *ptr;
				const int offset = w * samplesPerPixel;
				
				for (y = 0; y < h; y++, row_ptr += offset, row += bytesPerRow) {
					memcpy(row_ptr, row, offset);
				}
				
			}
			
		} else { // bitsPerPixel == 32
		
			// convert from ARGB to RGB
			int x, y;
			
			for (y = 0; y < h; y++) {
				unsigned char* row = &bytes[y * bytesPerRow];
				for (x = 0; x < w; x++) {
					(*ptr)[3*(y*w + x) + 0] = row[4*x + 1];
					(*ptr)[3*(y*w + x) + 1] = row[4*x + 2];
					(*ptr)[3*(y*w + x) + 2] = row[4*x + 3];
				}
			}
			
		}
	}

	[myPool release];
	
	return TRUE;
}

void CocoaFreeImageMem(unsigned char* ptr) {
	
	if (ptr != NULL)
		free(ptr);
}
