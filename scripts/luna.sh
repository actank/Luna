#!/bin/bash
for ((i=20;i<=30;i++)); do
	job_name=DataFilter_201309$i
	echo "Job: $job_name starting"
   	bash ~/Luna/scripts/filter.sh DataFilter 201309$i
   	touch ~/Luna/data/201309$i/job.done
done


