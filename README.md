Wide-Field Plate Database (WFPDB) is the basic source of data for the wide-field (larger or equal of 1 square degree) astronomical photographic plates obtained with professional telescopes world-wide (see wfpdb.org). It consists of four parts: Catalogue of Wide-Field Plate Archives (CWFPAs); Catalogue of Wide-Field Plate Indexes (CWFPIs) with regularly updated version (now for 563600 plates); Data Bank of digitized plate images (with low resolution for quick plate visualization and easy on-line access, and with high resolution aiming photometric and astrometric measurements); Links to on-line services and cross-correlation with other existing catalogues 
and journals.

Flexible Image Transport System or FITS is a digital file format used to store, transmit, and manipulate scientific and other images (fits.gsfc.nasa.gov). A major feature of the FITS format is that image meta-data is stored in a human-readable ASCII header. Each FITS file consists of a header containing ASCII card images (80 character fixed-length strings) that carry keyword/value pairs and an image data block.

Creation of complete FITS header is an important part of plates digitization. Because the photographic plates are a specific type of astronomical observations, we attempt to make a standard of the keywords in the FITS header, specially for astronomical photographic plates. The corresponding software implementation for this standard is also created. It is written in C++ using Qt cross platform application. When we generate FITS header for scanned plates, WFPDB catalogues are used for obtaining the most important meta-data for the plates. 

For MS Windows add
CONFIG += console
in .pro file.

[N. Kirov, M. Tsvetkov, K. Tsvetkova, Software Tools for Digitization of Astronomical Photographic Plates, Serdica Journal of Computing, 6 (1), (2012), 67-76.](http://nikolay.kirov.be/zip/067-076.pdfN)

[Kirov, M. Tsvetkov, K. Tsvetkova, Technology for digitization of astronomical photographic plates, In: Proceedings of the 8th Annual International Conference on Computer Science and Education in Computer Science, 5–8 July 2012, Boston, USA, 109-114.](http://nikolay.kirov.be/zip/nkirov_boston_updated.pdf)


