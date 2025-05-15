from generate_image_class import GenerateImage
from time import sleep

#Important: First, make a turn with dark_image(). This will save a dark_frame array as dark_frame.npy file. After that, you can start with read_image.
# Per default, dark_frame_array is set to only zeros.
image_reader = GenerateImage(port='/dev/ttyACM0')


#for cleaning dead pixels:
kernel_size = 5
thresh_fraction = 0.85

#for edge detection:
gaussian_kernel=(5, 5)
sigma=1.0
low_thresh=200
high_thresh=250

mode = int(input(   "DarkFrame = 0,\n"
                    "Unprocessed ImageRead = 1,\n"
                    "Processed ImageRead = 2,\n"
                    "Processed ImageRead + EdgeCanny = 3.\n"))
print("Mode:", mode)

if mode==0:
    image_reader.dark_frame()
    print("Done.")

elif mode==1:
    image_reader.light_frame()
    print("Done.")

elif mode==2:
    #image_reader.gain_calibration()
    print("Done.")

elif mode==3:
    image_reader.read_image(load=False)
    image_reader.apply_flat_field_correction()
    # image_reader.clean_dead_pixels()
    image_reader.show_image()
    image_reader.save_image()
    print("Done.")

while (image_reader.ser.is_open):
    print("force closing serial")
    image_reader.ser.close()


'''#Dark Frame
if mode==0:
    print("Creating Dark Fram Image...")
    image_reader.dark_frame(add_to_average)
    print("Dark Frame Image Created!")

#Unprocessed Image
elif mode==1:
    print("Reading Unprocessed Image...")
    image_reader.read_image()
    print("Image Read Done.")
    image_reader.show_image()

#Processed Image
elif mode==2:
    print("Reading and Processing Image...")
    image_reader.read_image()
    image_reader.clean_dead_pixels(kernel_size, thresh_fraction)
    print("Processing Done.")
    image_reader.show_image()

#Processed Image & Edge Canny
elif mode==3:
    print("The whole user experience...")
    image_reader.read_image()
    image_reader.clean_dead_pixels(kernel_size, thresh_fraction)
    image_reader.edge_detection(gaussian_kernel, sigma, low_thresh, high_thresh)
    print("Done.")
    image_reader.show_image()'''