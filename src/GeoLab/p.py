import os.path


AM = [7738,7749,5292,7756,7761,7762,7763,7774,7691,7803,7813,7818,7833,7834,7841,7853,7858,7863,7872,7355,7892,7381,7976,7928,7937,7647]

for lab in range(1,5+1):
	for am in AM:
		fname = "GeoLab_" +  "{0:02d}".format(lab) + "_" + str(am) + ".pdf"
		if os.path.isfile(fname) == False:
			print "%d %d" % (lab, am)
			
			