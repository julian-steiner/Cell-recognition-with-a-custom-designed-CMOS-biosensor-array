from generate_image_class import GenerateImage

#Important: First, make a turn with dark_image(). This will save a dark_frame array as dark_frame.npy file. After that, you can start with read_image.
# Per default, dark_frame_array is set to zeros.
image_reader = GenerateImage()

mode = int(input("DarkFrame = 0 / ImageRead = 1: "))
print("Mode" + mode)

if mode==0:
    image_reader.dark_frame()
else:
    image_reader.read_image()
    image_reader.show_image()

#B06-B07
#T03-T04