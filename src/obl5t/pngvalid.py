from PIL import Image
import argparse

def process_file(fname):
    v_image = Image.open(fname)
    v_image.verify()

def main():
    parser = argparse.ArgumentParser(description='PNG validator')
    parser.add_argument('file', help='The PMG file to be validated')
    args = parser.parse_args()
    process_file(args.file)
        
main()
